# Virtual Os ( In C++ And wxWidgets )
- ### The Single Developer Virtual Os made for simulation purposes and practice
## Current Features
- Has its own programming language which is still so basic with little features
- Simple gui in the programming language
- Ability to use the gui or the console with just changing one word
- Interpreters from an intermediate language
- A fast and less memory consuming virtual thing
- Still very basic and has a basic desktop system
- For windows but built in wx widgets and cross platform libraries so can be compiled other operating systems
- Http/Https Support
- File Download with 1 function support
- Supports Webp imgs from the internet through downloading them and using them

## Gui Code Example
```
gui

call sleep(100)
call download_file("https://cdn.discordapp.com/avatars/765939325136928799/b0836d88d9a66a0f4c67ac0fcd2d6c4d.webp?size=128", "skalpha.webp", false)
variable header = call create_panel(window, 0, 40, 1366, 60)
variable side_bar = call create_panel(window, 0, 60 + 40, 300, 636 - 80)
variable main = call create_panel(window, 300, 60 + 40, 1366 - 300, 636)
variable header_txt = call create_label(header, "Stack Hasn't OverFlowed" , 30, 5, 200, 40, 30)
variable profile_area = call create_panel(window, 0, 60 + 40 + (636 - 80), 300, 80)
variable pfp = call create_image(profile_area, "skalpha.webp", 10, (80 / 2) - (50 / 2), 50, 50)
variable name = call create_label(profile_area, "Oogway", 70, 19, 150, 40, 15)
variable status = call create_label(profile_area, "online", 70, 44, 150, 25, 13)
variable flicker_hider = call create_panel(profile_area, 10 - 3, (80 / 2) - (50 / 2) - 3, 56, 56)
variable status_ball = call create_panel(profile_area, 47, ((80 / 2) - (50 / 2)) + 37, 15, 15)
variable centerized_thing = 0

call bind_event_to_handle(pfp, "click_event", lambda(){
    variable window_s = call get_window_size(window)
    reinit centerized_thing = call create_panel(window, (idx window_s[0] / 2) - (500 / 2), 
        (idx window_s[1] / 2) - (500 / 2), 500, 500)
    variable profile_img = call create_image(centerized_thing, "skalpha.webp", 20, 20, 70, 70)
    variable profile_img_flicker_reducer = call create_panel(centerized_thing, 20 - 3, 20 - 3, 76, 76)
    call set_window_border_color(profile_img_flicker_reducer, 30, 30 , 32, 255)

    call set_window_border_radius(profile_img, 35)
    call set_window_border_radius(profile_img_flicker_reducer, 35)

    call set_window_bk_color(centerized_thing, 35, 35, 40, 255)
    call set_window_bk_color(profile_img_flicker_reducer, 0, 0, 0, 0)

    call set_window_border_color(profile_img_flicker_reducer, 35, 35, 40, 255)

    call set_window_border_color(centerized_thing, 0, 0, 0, 0)
    call show_window(centerized_thing)
    call show_window(profile_img)
    call show_window(profile_img_flicker_reducer)


})

call set_window_bk_color(window, 18, 18, 19, 255)

call set_window_border_color(header, 0, 0 , 0, 0)
call set_window_border_color(side_bar, 0, 0 , 0, 0)
call set_window_border_color(main, 0, 0 , 0, 0)
call set_window_border_color(profile_area, 0, 0 , 0, 0)
call set_window_border_color(flicker_hider, 30, 30 , 32, 255)
call set_window_border_color(status_ball, 0, 0, 0, 0)

call set_window_bk_color(header, 30, 30, 32, 255)
call set_window_bk_color(side_bar, 30, 30, 32, 255)
call set_window_bk_color(main, 30, 30, 32, 255)
call set_window_bk_color(profile_area, 30, 30, 32, 255)
call set_window_bk_color(status_ball, 0, 255, 0, 255)
call set_window_bk_color(flicker_hider, 0, 0, 0, 0)

call set_window_font(header_txt, "Arial", 27, 650)
call set_window_txt_color(header_txt, 255, 255, 255, 255)
call set_window_txt_color(name, 255, 255, 255, 255)
call set_window_txt_color(status, 60, 60, 60, 255)

call set_window_border_radius(pfp, 25)
call set_window_border_radius(status_ball, 8)
call set_window_border_weight(status_ball, 0)
call set_window_border_radius(flicker_hider, 25)

call show_window(header)
call show_window(header_txt)
call show_window(side_bar)
call show_window(main)
call show_window(profile_area)
call show_window(pfp)
call show_window(name)
call show_window(status)
call show_window(flicker_hider)
call show_window(status_ball)

call interrupt()
```
## Try it your self !!!
