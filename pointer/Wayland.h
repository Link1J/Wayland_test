#pragma once

#include <wayland-client.h>
#include <wayland-cursor.h>
#include <wayland-client-protocol.h>
#include <xdg-shell-client-protocol.h>
#include <xdg-decoration-client-protocol.h>

#include <string>
#include <unordered_map>

namespace Wayland
{
	namespace Functions
	{
		namespace WL_Registry
		{
			void Global(void*, wl_registry*, uint32_t, const char*, uint32_t);
			void Global_Remove(void*, wl_registry*, uint32_t);
		}

		namespace WL_Seat
		{
			void Capabilities(void*, wl_seat*, uint32_t);
			void Name(void*, wl_seat*, const char*);
		}

		namespace WL_Shell_Surface
		{
			void Ping(void*, wl_shell_surface*, uint32_t);
			void Configure(void*, wl_shell_surface*, uint32_t, int32_t, int32_t);
		}

		namespace XDG_WM_Base 
		{
			void Ping(void*, xdg_wm_base*, uint32_t);
		}

		namespace XDG_Surface
		{
			void Configure(void*, xdg_surface*, uint32_t);
		}

		namespace XDG_Toplevel
		{
			void Configure(void*, xdg_toplevel*, int32_t, int32_t, wl_array*);
			void Close(void*, xdg_toplevel*);
		}

		namespace ZXDG_Toplevel_Decoration_V1
		{
			void Configure(void*, zxdg_toplevel_decoration_v1*, uint32_t);
		}

		namespace WL_Pointer
		{
			void Enter(void*, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t);
			void Leave(void*, struct wl_pointer*, uint32_t, struct wl_surface*);
			void Motion(void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t);
			void Button(void*, struct wl_pointer*, uint32_t, uint32_t, uint32_t, uint32_t);
			void Axis(void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t);
			void Frame(void*, struct wl_pointer*);
			void Axis_Source(void*, struct wl_pointer*, uint32_t);
			void Axis_Stop(void*, struct wl_pointer*, uint32_t, uint32_t);
			void Axis_Discrete(void*, struct wl_pointer*, uint32_t, int32_t);
		}
	}
			
	class Window
	{
		wl_surface                 * surface_wl         ;
		wl_shell_surface           * shell_surface      ;
		xdg_surface                * surface_xdg        ;
		xdg_toplevel               * toplevel           ;
		zxdg_toplevel_decoration_v1* toplevel_decoration;
		wl_buffer                  * shm_buffer         ;
		wl_shm_pool                * pool               ;

		wl_pointer                 * pointer            ;
		wl_keyboard                * keyboard           ;
		wl_callback                * frame_cb           ;
		wl_cursor_image            * cursor_image       ;
		wl_buffer                  * cursor_buffer      ;
		wl_surface                 * cursor_surface     ;

		struct PointerData
		{
			double xPos, yPos;
			bool left, middle, right;
		};

		std::unordered_map<wl_pointer*,PointerData> pointerData;

		bool  isOpen  = false;
        bool  resized = true ;
		int   x              ;
		int   y              ;
		int   width          ;
		int   height         ;
		char* shm_data       ;  
		int   fd             ;

	public:
		friend void Functions::WL_Registry::Global(void*, wl_registry*, uint32_t, const char*, uint32_t);
		friend void Functions::WL_Seat::Capabilities(void*, wl_seat*, uint32_t);
		friend void Functions::WL_Seat::Name(void*, wl_seat*, const char*);
		friend void Functions::WL_Shell_Surface::Ping(void*, wl_shell_surface*, uint32_t);
		friend void Functions::WL_Shell_Surface::Configure(void*, wl_shell_surface*, uint32_t, int32_t, int32_t);
		friend void Functions::XDG_WM_Base::Ping(void*, xdg_wm_base*, uint32_t);
		friend void Functions::XDG_Surface::Configure(void*, xdg_surface*, uint32_t);
		friend void Functions::XDG_Toplevel::Configure(void*, xdg_toplevel*, int32_t, int32_t, wl_array*);
		friend void Functions::XDG_Toplevel::Close(void*, xdg_toplevel*);
		friend void Functions::ZXDG_Toplevel_Decoration_V1::Configure(void*, zxdg_toplevel_decoration_v1*, uint32_t);
		friend void Functions::WL_Pointer::Enter(void*, struct wl_pointer*, uint32_t, struct wl_surface*, wl_fixed_t, wl_fixed_t);
		friend void Functions::WL_Pointer::Leave(void*, struct wl_pointer*, uint32_t, struct wl_surface*);
		friend void Functions::WL_Pointer::Motion(void*, struct wl_pointer*, uint32_t, wl_fixed_t, wl_fixed_t);
		friend void Functions::WL_Pointer::Button(void*, struct wl_pointer*, uint32_t, uint32_t, uint32_t, uint32_t);
		friend void Functions::WL_Pointer::Axis(void*, struct wl_pointer*, uint32_t, uint32_t, wl_fixed_t);
		friend void Functions::WL_Pointer::Frame(void*, struct wl_pointer*);
		friend void Functions::WL_Pointer::Axis_Source(void*, struct wl_pointer*, uint32_t);
		friend void Functions::WL_Pointer::Axis_Stop(void*, struct wl_pointer*, uint32_t, uint32_t);
		friend void Functions::WL_Pointer::Axis_Discrete(void*, struct wl_pointer*, uint32_t, int32_t);

		void Init(int width, int height, int x, int y, bool show = true, Window* parent = nullptr, bool noDecoration = false, bool noTaskbarIcon = false, bool topmost = false);

		void  Poll				(						);
		void* GetWindowHandler	(						);
		void  ChangeWindowText	(std::string text		);
		bool  IsOpen			(						);
		void  SetSize			(int width, int height	);
		void  SetPosition		(int x, int y			);
		void  SetFocus			(						);
		void  SetAlpha			(float alpha			);
		void  Show				(bool show				);
		bool  IsFocus			(						);
		bool  IsMinimized		(						);
		
		static void SetupStatics();
		void create_buffer();
	};
}
