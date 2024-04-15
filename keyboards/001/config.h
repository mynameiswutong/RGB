#pragma once

#define CAPS_LOCK_LED_INDEX  70
#ifdef OPENRGB_ENABLE
    #define OPENRGB_DIRECT_MODE_USE_UNIVERSAL_BRIGHTNESS
#endif

/*//灯效模式说明
solid_color:  静态单色，不支持速度控制
alphas_mods:  静态双色，速度控制副色调的色调
gradient_up_down:  静态上下渐变，速度控制渐变程度
gradient_left_right:  静态左右渐变，速度控制渐变程度
breathing:  单色呼吸灯
band_sat:  饱和度分带
band_val:  亮度分带
band_pinwheel_sat:  饱和度分带风车
band_pinwheel_val:  亮度分带风车
band_spiral_sat:  饱和度分带螺旋
band_spiral_val:  亮度分带螺旋
cycle_all:  单色渐变
cycle_left_right:  左右渐变
cycle_up_down:  上下渐变
rainbow_moving_chevron:  彩虹人字形移动
cycle_out_in:  向内渐变
cycle_out_in_dual:  双色向内渐变
cycle_pinwheel:  风车渐变
cycle_spiral:  螺旋渐变
dual_beacon:  双色信标
rainbow_beacon:  彩虹信标
rainbow_pinwheels:  彩虹风车
flower_blooming:  花朵绽放
raindrops:  雨滴
jellybean_raindrops:  糖豆雨/彩虹雨
hue_breathing:  色调呼吸
hue_pendulum:  摆钟
hue_wave:  波浪
pixel_rain:  像素雨
pixel_flow:  像素流动
pixel_fractal:  像素分形
typing_heatmap:  打字热力图
digital_rain:  数字雨/黑客字符流
solid_reactive_simple:  单色脉冲
solid_reactive:  静态单色，按键触发副色调脉冲，然后淡出
solid_reactive_wide:  按键周围的色调和亮度脉冲，然后淡出
solid_reactive_multiwide:  多个按键周围的色调和亮度脉冲，然后淡出
solid_reactive_cross:  单键脉冲
solid_reactive_multicross:  多键脉冲
solid_reactive_nexus:  单键十字脉冲
solid_reactive_multinexus:  多键十字脉冲
splash:  单键溅射
multisplash:  多重溅射
solid_splash:  单键单色溅射
solid_multisplash:  多重单色溅射
starlight:  随机亮度，保持颜色
starlight_dual_sat:  随机亮度，色调+- 30
starlight_dual_hue:  随机亮度，饱和度+- 30
riverflow:  河流流动
openrgb_direct: OpenRGB软件预留效果。
signalrgb: signalrgb软件预留效果。
*/