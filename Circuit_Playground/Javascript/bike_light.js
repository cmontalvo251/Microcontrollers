let bright = 255
let FLASH = 0
let color = light.rgb(255, 255, 255)
let MODE = 0
let COLORMODE = 0
// Change Color from B Button Press
input.buttonB.onEvent(ButtonEvent.Click, function () {
    COLORMODE += 1
    switch (COLORMODE) {
        case 0:
            color = light.rgb(255, 255, 255)
            break
        case 1:
            color = light.rgb(255, 0, 0)
            break
        case 2:
            color = light.rgb(0, 255, 0)
            break
        case 3:
            color = light.rgb(0, 0, 255)
            COLORMODE = -1
            break
    }
})
// Change Mode from A Button Press
input.buttonA.onEvent(ButtonEvent.Click, function () {
    MODE += 1
    FLASH = 0
    if (MODE == 0) {
        bright = 255
    } else if (MODE == 1) {
        bright = 20
    } else if (MODE == 2) {
        bright = 255
        FLASH = 1
    } else {
        bright = 0
        MODE = -1
    }
})
loops.forever(function () {
    light.setAll(color)
    light.setBrightness(bright)
    if (FLASH) {
        loops.pause(100)
        light.setBrightness(0)
        loops.pause(100)
    }
})

