#include "Wayland.h"

#include <cstring>
#include <cstdint>

#include <stdexcept>
#include <iostream>
#include <vector>

#include <fmt/format.h>

#include <fcntl.h>
#include <sys/mman.h>
#include <errno.h>
#include <unistd.h>

#define WL_ARRAY_FOR_EACH(pos, array, type) \
	for (pos = (type)(array)->data; \
	     (const char *) pos < ((const char *) (array)->data + (array)->size); \
	     (pos)++)

static int set_cloexec_or_close(int fd)
{
	long flags;

	if (fd == -1)
			return -1;

	flags = fcntl(fd, F_GETFD);
	if (flags == -1)
		goto err;

	if (fcntl(fd, F_SETFD, flags | FD_CLOEXEC) == -1)
		goto err;

	return fd;

err:
	close(fd);
	return -1;
}

static int create_tmpfile_cloexec(char *tmpname)
{
	int fd;

#ifdef HAVE_MKOSTEMP
	fd = mkostemp(tmpname, O_CLOEXEC);
	if (fd >= 0)
			unlink(tmpname);
#else
	fd = mkstemp(tmpname);
	if (fd >= 0) {
		fd = set_cloexec_or_close(fd);
		unlink(tmpname);
	}
#endif

	return fd;
}

int os_create_anonymous_file(off_t size)
{
	static const char templatea[] = "/weston-shared-XXXXXX";
	const char *path;
	char *name;
	int fd;

	path = getenv("XDG_RUNTIME_DIR");
	if (!path) {
		errno = ENOENT;
		return -1;
	}

	name = (char*)malloc(strlen(path) + sizeof(templatea));
	if (!name)
		return -1;
	strcpy(name, path);
	strcat(name, templatea);

	fd = create_tmpfile_cloexec(name);

	free(name);

	if (fd < 0)
		return -1;

	if (ftruncate(fd, size) < 0) {
		close(fd);
		return -1;
	}

	return fd;
}

namespace Wayland
{
	wl_display*                 display;
	wl_registry*                registry;
	wl_compositor*              compositor;
	wl_shell*                   shell;
	xdg_wm_base*                wm_base;
	wl_seat*                    seat;
	wl_shm*                     shm;
	zxdg_decoration_manager_v1* decoration_manager;

	namespace Functions
	{
		namespace WL_Registry
		{
			#define Bind(T, var)\
				if (!strcmp(interface, #T)) \
					var = (T*)wl_registry_bind(wl_registry, name, &T##_interface, version)

			void Global(void* data, wl_registry* wl_registry, uint32_t name, const char* interface, uint32_t version)
			{
				std::cout << fmt::format("{}: {} {}\n", name, interface, version);

				Bind(wl_compositor             , compositor        );
				Bind(wl_shell                  , shell             );
				Bind(wl_seat                   , seat              );
				Bind(wl_shm                    , shm               );
				Bind(xdg_wm_base               , wm_base           );
				Bind(zxdg_decoration_manager_v1, decoration_manager);
			}

			#undef Bind

			void Global_Remove(void* data, wl_registry* wl_registry, uint32_t name)
			{
			}
		}

		namespace WL_Seat
		{
			void Capabilities(void* data, wl_seat* wl_seat, uint32_t capabilities)
			{
			}

			void Name(void* data, wl_seat* wl_seat, const char* name)
			{
			}
		}

		namespace WL_Shell_Surface
		{
			void Ping(void* data, wl_shell_surface* wl_shell_surface, uint32_t serial)
			{
				wl_shell_surface_pong(wl_shell_surface, serial);
			}

			void Configure(void* data, wl_shell_surface* wl_shell_surface, uint32_t edges, int32_t width, int32_t height)
			{
				Window* window = (Window*)data;

				if (width != 0 && height != 0)
				{
					window->width  = width ;
					window->height = height;
				}
			}
		}

		namespace XDG_WM_Base 
		{
			void Ping(void* data, xdg_wm_base* xdg_wm_base, uint32_t serial)
			{
				xdg_wm_base_pong(xdg_wm_base, serial);
			}
		}

		namespace XDG_Surface
		{
			void Configure(void* data, xdg_surface* xdg_surface, uint32_t serial)
			{
				xdg_surface_ack_configure(xdg_surface, serial);
			}
		}

		namespace XDG_Toplevel
		{
			void Configure(void* data, xdg_toplevel* xdg_toplevel, int32_t width, int32_t height, wl_array* states)
			{
				Window* window = (Window*)data;

				std::vector<std::string> state_strings = {
					"",
					"XDG_TOPLEVEL_STATE_MAXIMIZED",
					"XDG_TOPLEVEL_STATE_FULLSCREEN",
					"XDG_TOPLEVEL_STATE_RESIZING",
					"XDG_TOPLEVEL_STATE_ACTIVATED",
					"XDG_TOPLEVEL_STATE_TILED_LEFT",
					"XDG_TOPLEVEL_STATE_TILED_RIGHT",
					"XDG_TOPLEVEL_STATE_TILED_TOP",
					"XDG_TOPLEVEL_STATE_TILED_BOTTOM"
				};
                
                bool resizing = false;
				
				std::cout << fmt::format("Size: {},{}\n", width, height);
				std::cout << fmt::format("State Changes: {}, {}\n", states->size, states->alloc);
				
				const uint32_t *state;
				WL_ARRAY_FOR_EACH(state, states, const uint32_t*)
				{
					std::string state_string = "";
					if (*state < state_strings.size())
						state_string = state_strings[*state];
					std::cout << fmt::format("\t{}: {}\n", *state, state_string);
                    
                    if (*state == 3)
                        resizing = true;
                    if (*state == 1)
                        resizing = true;
                    if (*state == 5)
                        resizing = true;
                    if (*state == 6)
                        resizing = true;
                    if (*state == 7)
                        resizing = true;
                    if (*state == 8)
                        resizing = true;
				}
				
				if (width != 0 && height != 0 && resizing)
				{
                    window->width   = width ;
                    window->height  = height;
                    window->resized = true  ;
				}
			}

			void Close(void* data, xdg_toplevel* xdg_toplevel)
			{
				Window* window = (Window*)data;
				window->isOpen = false;
			}
		}

		namespace ZXDG_Toplevel_Decoration_V1
		{
			void Configure(void* data, zxdg_toplevel_decoration_v1* zxdg_toplevel_decoration_v1, uint32_t mode)
			{
				std::vector<std::string> modes = {"","ZXDG_TOPLEVEL_DECORATION_V1_MODE_CLIENT_SIDE","ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE"};
				std::string mode_string = modes[mode];
				std::cout << fmt::format("Mode: {}\n", mode_string);
			}
		}

		namespace WL_Shm
		{
			void Format(void* data, wl_shm* wl_shm, uint32_t format)
			{
				std::vector<std::string> formats = {
					"WL_SHM_FORMAT_ARGB8888",
					"WL_SHM_FORMAT_XRGB8888",
					"WL_SHM_FORMAT_C8",
					"WL_SHM_FORMAT_RGB332",
					"WL_SHM_FORMAT_BGR233",
					"WL_SHM_FORMAT_XRGB4444",
					"WL_SHM_FORMAT_XBGR4444",
					"WL_SHM_FORMAT_RGBX4444",
					"WL_SHM_FORMAT_BGRX4444",
					"WL_SHM_FORMAT_ARGB4444",
					"WL_SHM_FORMAT_ABGR4444",
					"WL_SHM_FORMAT_RGBA4444",
					"WL_SHM_FORMAT_BGRA4444",
					"WL_SHM_FORMAT_XRGB1555",
					"WL_SHM_FORMAT_XBGR1555",
					"WL_SHM_FORMAT_RGBX5551",
					"WL_SHM_FORMAT_BGRX5551",
					"WL_SHM_FORMAT_ARGB1555",
					"WL_SHM_FORMAT_ABGR1555",
					"WL_SHM_FORMAT_RGBA5551",
					"WL_SHM_FORMAT_BGRA5551",
					"WL_SHM_FORMAT_RGB565",
					"WL_SHM_FORMAT_BGR565",
					"WL_SHM_FORMAT_RGB888",
					"WL_SHM_FORMAT_BGR888",
					"WL_SHM_FORMAT_XBGR8888",
					"WL_SHM_FORMAT_RGBX8888",
					"WL_SHM_FORMAT_BGRX8888",
					"WL_SHM_FORMAT_ABGR8888",
					"WL_SHM_FORMAT_RGBA8888",
					"WL_SHM_FORMAT_BGRA8888",
					"WL_SHM_FORMAT_XRGB2101010",
					"WL_SHM_FORMAT_XBGR2101010",
					"WL_SHM_FORMAT_RGBX1010102",
					"WL_SHM_FORMAT_BGRX1010102",
					"WL_SHM_FORMAT_ARGB2101010",
					"WL_SHM_FORMAT_ABGR2101010",
					"WL_SHM_FORMAT_RGBA1010102",
					"WL_SHM_FORMAT_BGRA1010102",
					"WL_SHM_FORMAT_YUYV",
					"WL_SHM_FORMAT_YVYU",
					"WL_SHM_FORMAT_UYVY",
					"WL_SHM_FORMAT_VYUY",
					"WL_SHM_FORMAT_AYUV",
					"WL_SHM_FORMAT_NV12",
					"WL_SHM_FORMAT_NV21",
					"WL_SHM_FORMAT_NV16",
					"WL_SHM_FORMAT_NV61",
					"WL_SHM_FORMAT_YUV410",
					"WL_SHM_FORMAT_YVU410",
					"WL_SHM_FORMAT_YUV411",
					"WL_SHM_FORMAT_YVU411",
					"WL_SHM_FORMAT_YUV420",
					"WL_SHM_FORMAT_YVU420",
					"WL_SHM_FORMAT_YUV422",
					"WL_SHM_FORMAT_YVU422",
					"WL_SHM_FORMAT_YUV444",
					"WL_SHM_FORMAT_YVU444"
				};

				if (format < formats.size())
					std::cout << fmt::format("Format Supported: {}\n", formats[format]);
				else
					std::cout << fmt::format("Unknown Format: {}\n", format);
			}
		}
	}

	namespace Listeners
	{
		wl_registry_listener wl_registry
		{
			.global = Functions::WL_Registry::Global,
			.global_remove = Functions::WL_Registry::Global_Remove
		};

		wl_seat_listener wl_seat
		{
			.capabilities = Functions::WL_Seat::Capabilities,
			.name = Functions::WL_Seat::Name
		};

		wl_shell_surface_listener wl_shell_surface
		{
			.ping = Functions::WL_Shell_Surface::Ping,
			.configure = Functions::WL_Shell_Surface::Configure,
		};

		xdg_wm_base_listener xdg_wm_base
		{
			.ping = Functions::XDG_WM_Base::Ping
		};

		xdg_surface_listener xdg_surface
		{
			.configure = Functions::XDG_Surface::Configure
		};

		xdg_toplevel_listener xdg_toplevel
		{
			.configure = Functions::XDG_Toplevel::Configure,
			.close = Functions::XDG_Toplevel::Close
		};

		zxdg_toplevel_decoration_v1_listener zxdg_toplevel_decoration_v1
		{
			.configure = Functions::ZXDG_Toplevel_Decoration_V1::Configure,
		};

		wl_shm_listener wl_shm
		{
			.format = Functions::WL_Shm::Format
		};
	}

	void Window::SetupStatics()
	{
		display = wl_display_connect(NULL);
		if (display == nullptr)
			throw std::runtime_error("No display found");

		registry = wl_display_get_registry(display);

		wl_registry_add_listener(registry, &Listeners::wl_registry, nullptr);
		wl_display_roundtrip(display);

		if (compositor == nullptr                      ) throw std::runtime_error("No compositor found");
		if (seat       == nullptr                      ) throw std::runtime_error("No seat found"      );
		if (shm        == nullptr                      ) throw std::runtime_error("No shm found"       );
		if (shell      == nullptr && wm_base == nullptr) throw std::runtime_error("No shell found"     );

		if (wm_base != nullptr)
			xdg_wm_base_add_listener(wm_base, &Listeners::xdg_wm_base, nullptr);

		wl_seat_add_listener(seat, &Listeners::wl_seat, nullptr);
		wl_shm_add_listener(shm, &Listeners::wl_shm, nullptr);
	}

	void Window::Init(int width, int height, int x, int y, bool show, Window* parent, bool noDecoration, bool noTaskbarIcon, bool topmost)
	{
		// create a surface
		surface_wl = wl_compositor_create_surface(compositor);

		// create a shell surface
		if(wm_base)
		{
			surface_xdg = xdg_wm_base_get_xdg_surface(wm_base, surface_wl);
			xdg_surface_add_listener(surface_xdg, &Listeners::xdg_surface, this);

			toplevel = xdg_surface_get_toplevel(surface_xdg);
			xdg_toplevel_add_listener(toplevel, &Listeners::xdg_toplevel, this);
			xdg_toplevel_set_title(toplevel, "SHM-Test");
			xdg_toplevel_set_app_id(toplevel, "Test App");
		}
		else if (shell)
		{
			shell_surface = wl_shell_get_shell_surface(shell, surface_wl);
			wl_shell_surface_add_listener(shell_surface, &Listeners::wl_shell_surface, this);
			wl_shell_surface_set_title(shell_surface, "SHM-Test");
			wl_shell_surface_set_class(shell_surface, "Test App");
			wl_shell_surface_set_toplevel(shell_surface);
		}

		if (decoration_manager && toplevel)
		{ 
			toplevel_decoration = zxdg_decoration_manager_v1_get_toplevel_decoration(decoration_manager, toplevel);
			zxdg_toplevel_decoration_v1_add_listener(toplevel_decoration, &Listeners::zxdg_toplevel_decoration_v1, this);
			zxdg_toplevel_decoration_v1_set_mode(toplevel_decoration, ZXDG_TOPLEVEL_DECORATION_V1_MODE_SERVER_SIDE);
		}

		this->x      = x     ;
		this->y      = y     ;
		this->width  = width ;
		this->height = height;

		wl_surface_commit(surface_wl);
		wl_display_roundtrip(display);

		isOpen = true;
	}

	void* Window::GetWindowHandler()
	{
		return surface_wl;
	}

	void Window::Poll()
	{
        wl_display_roundtrip(display);
        
        if (resized)
        {
            std::cout << fmt::format("Current Size: {},{}\n", width, height);
            
            create_buffer();

			const int OFFSET_RED   = 2;
			const int OFFSET_GREEN = 1;
			const int OFFSET_BLUE  = 0;
			const int OFFSET_ALPHA = 3;
            
            for (int x = 0; x < width; x++)
            {
                for (int y = 0; y < height; y++)
                {
					int offset = (x + y * width) * 4;

                    shm_data[offset + OFFSET_RED  ] = (x / (float)width ) * 256;
                    shm_data[offset + OFFSET_GREEN] = (y / (float)height) * 256;
                    shm_data[offset + OFFSET_BLUE ] = 0x00;
                    shm_data[offset + OFFSET_ALPHA] = 0xFF; //rand() % 256;
                }
            }

            wl_surface_attach(surface_wl, shm_buffer, x, y);
            wl_surface_damage(surface_wl, 0, 0, width, height);
            wl_surface_commit(surface_wl);
            
            resized = false;
        }
	}

	bool Window::IsOpen()
	{
		return isOpen;
	}

	void Window::ChangeWindowText(std::string text)
	{
	}

	void Window::SetSize(int width, int height)
	{
	}
	
	void Window::SetPosition(int x, int y)
	{
	}

	void Window::SetFocus()
	{
	}

	void Window::SetAlpha(float alpha)
	{

	}

	void Window::Show(bool show)
	{

	}

	bool Window::IsFocus()
	{
		return false;
	}
	
	bool Window::IsMinimized()
	{
		return false;
	}

	void Window::create_buffer() 
	{
        if (pool)
        {
            wl_shm_pool_destroy(pool);
            close(fd);
        }
        
		int stride = width * 4; // 4 bytes per pixel
		int size = stride * height;

		fd = os_create_anonymous_file(size);
		if (fd < 0) {
			fprintf(stderr, "creating a buffer file for %d B failed: %m\n", size);
			exit(1);
		}
	
		shm_data = (char*)mmap(NULL, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
		if (shm_data == MAP_FAILED) {
			fprintf(stderr, "mmap failed: %m\n");
			close(fd);
			exit(1);
		}

		pool = wl_shm_create_pool(shm, fd, size);
		shm_buffer = wl_shm_pool_create_buffer(pool, 0, width, height, stride, WL_SHM_FORMAT_ARGB8888);
	}
}
