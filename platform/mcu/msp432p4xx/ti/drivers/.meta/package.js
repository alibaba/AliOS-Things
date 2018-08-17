"use strict";
/*global exports*/

/*
 * pinmux/pinmux/deviceData/MSP432P401R/templates/Drivers/ contains the
 * initial Board generation prototype
 */
exports = {
    name: "/ti/drivers",
    displayName: "TI Drivers",

    /* issue: the package files list should be computed */
    files: [ /* All package files */
        "UART.syscfg.js",
        "uart/UARTMSP432.syscfg.js",
        "uart/UARTMSP432.Board.c.xdt",
        "uart/UARTMSP432.Board.h.xdt",
        "uart/UARTCC32XX.syscfg.js",
        "uart/UARTCC32XX.Board.c.xdt",
        "uart/UARTCC32XX.Board.h.xdt",

        "Power.syscfg.js",
        "power/PowerMSP432.syscfg.js",
        "power/PowerMSP432.Board.c.xdt",
        "power/PowerMSP432.Board.h.xdt",
        "power/PowerCC32XX.syscfg.js",
        "power/PowerCC32XX.Board.c.xdt",
        "power/PowerCC32XX.Board.h.xdt",

        "Watchdog.syscfg.js",
        "watchdog/WatchdogMSP432.syscfg.js",
        "watchdog/WatchdogMSP432.Board.c.xdt",
        "watchdog/WatchdogMSP432.Board.h.xdt",
        "watchdog/WatchdogCC32XX.syscfg.js",
        "watchdog/WatchdogCC32XX.Board.c.xdt",
        "watchdog/WatchdogCC32XX.Board.h.xdt",

        "Timer.syscfg.js",
        "timer/TimerMSP432.syscfg.js",
        "timer/TimerMSP432_T32.syscfg.js",
        "timer/TimerMSP432_TA.syscfg.js",
        "timer/TimerMSP432.Board.c.xdt",
        "timer/TimerMSP432.Board.h.xdt",

        "NVS.syscfg.js",
        "nvs/NVSMSP432.syscfg.js",
        "nvs/NVSSPI25X.syscfg.js",

        "DMA.syscfg.js",

        /* root code generation templates, referenced by the product json */
        "templates/Board.c.xdt",
        "templates/Board.h.xdt"
    ]
};
