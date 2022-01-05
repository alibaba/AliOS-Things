## ESP Dispatcher
Most of audio products in the market have highly similar features, such as applying basically the same human-computer interaction mode, and using nearly identical cloud services provided by various cloud servers. What differentiates is only the key functions of a product and its logical control module, which means you can reuse the other functions of some existing products. Now, Espressif provides a new design, ESP_Dispatcher, to help reuse these functions.

Based on ESP_Dispatcher, product functions are split into event input module, logic control module and execution output module, with each module further subdivided into separate sub-functional units. Each sub-functional unit can be combined flexibly with other units or with customized logic control design.

ESP_Dispatcher helps you reuse the same functions of different products, which avoids repeated development and makes the existing product functions more reusable. Meanwhile, you can design differentiated functions and logic control module at your own will, making product development more flexible. In such way, you can shorten product development cycle, and speed up the timing to market and product iteration.

ESP_Dispatcher consists of a dispatcher and a series of services.

* The dispatcher adopts a separate-execution mechanism and executes functional units separately according to unit ID.

* The service is a highly-abstract functional class and only generates external input and output, such as providing executable functional units and generating service state events. Non-external tasks are handled internally in service. Currently, ESP_Dispatcher support 2 services:
    * Peripheral Service
        * Peripheral service is used to handle external tasks concerning GPIO, Touch, SDCARD, ADC, UART, Display, Wi-Fi and network configuration.
        * This service also can generate GPIO interrupts, Touch events, and provides management and control functions, such as network configuration control and LED display style management.

    * Audio Service
        * Audio service is used to handle protocol class functions, such as cloud service (DuerOS), DLNA and VoIP, and provides control-class interfaces and state events.

Each module and sub-functional unit in ESP_Dispatcher runs as a separate task, providing a structure that can simplify complex product styles and adapts to any product style.

A standard ESP_Dispatcher audio application block diagram as shown below.
<div align="center"><img src="../../docs/_static/esp_dispatcher_audio_app_diagram.png" alt ="ESP Dispatcher Audio Application Diagram" align="center" /></div>

There is an example in the folder of [esp_dispatcher_dueros](https://github.com/espressif/esp-adf/tree/master/examples/advanced_examples/esp_dispatcher_dueros).

