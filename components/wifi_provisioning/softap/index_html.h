const char *index_html_start = 
"<!DOCTYPE html>\n"\
"<html lang=\"en\">\n"\
"<head>\n"\
"    <meta charset=\"UTF-8\">\n"\
"    <meta name=\"viewport\" content=\"width=device-width,initial-scale=1.0, minimum-scale=1, maximum-scale=1,user-scalable=no\">\n"\
"    <title>WiFi</title>\n"\
"    <style>\n"\
"        html,body{\n"\
"            margin: 0;\n"\
"            padding: 0;\n"\
"            text-align: center;\n"\
"        }\n"\
"        .yun-title{\n"\
"            font-size: 22px;\n"\
"            margin-top: 55px;\n"\
"            color: #007CF7;\n"\
"        }\n"\
"        .yun-log{\n"\
"            width: 45%;\n"\
"            margin-top: 38px;\n"\
"        }\n"\
"        .yun-container{\n"\
"            padding: 0 10%;\n"\
"            margin-top: 40px;\n"\
"        }\n"\
"        select,input{\n"\
"            outline: none;\n"\
"            width: 100%;\n"\
"            border: none;\n"\
"            height: 34px;\n"\
"            padding: 0 35px;\n"\
"            box-sizing: border-box;\n"\
"            border-bottom: 1px solid #979797;\n"\
"            font-size: 13px;\n"\
"            color: #000000;\n"\
"            margin-bottom: 10px;\n"\
"			background: white;\n"\
"        }\n"\
"        select{\n"\
"            appearance:none;\n"\
"            -moz-appearance:none;\n"\
"            -webkit-appearance:none;\n"\
"        }\n"\
"        input::placeholder{\n"\
"            color: #AEAEAE;\n"\
"        }\n"\
"        .icon{\n"\
"            width: 18px;\n"\
"            height: 20px;\n"\
"        }\n"\
"        .yun-row{\n"\
"            position: relative;\n"\
"        }\n"\
"        .yun-icon-left{\n"\
"            position: absolute;\n"\
"            left: 0;\n"\
"            top: 5px;\n"\
"        }\n"\
"        .yun-icon-right{\n"\
"            position: absolute;\n"\
"            right: 0;\n"\
"            top: 5px;\n"\
"        }\n"\
"        .yun-suo{\n"\
"            width: 16px;\n"\
"        }\n"\
"        .yun-btn{\n"\
"            width: 100%;\n"\
"            height:44px;\n"\
"            border: none;\n"\
"            background:rgba(0,124,247,1);\n"\
"            box-shadow:0 1px 4px 1px rgba(0,124,247,0.4);\n"\
"            border-radius:22px;\n"\
"            font-size: 22px;\n"\
"            color: white;\n"\
"			outline: none;\n"\
"        }\n"\
"    </style>\n"\
"</head>\n"\
"<body>\n"\
"<div class=\"yun-title\">连接家庭Wi-Fi</div>\n"\
"<form name=\"form\" method=\"post\" onsubmit=\"return checkForm()\" class=\"yun-container\">\n"\
"    <div class=\"yun-row\">\n"\
"        <svg class=\"icon yun-icon-left\" width=\"200px\" height=\"200.00px\" viewBox=\"0 0 1024 1024\" version=\"1.1\">\n"\
"            <path fill=\"#AeAeAe\" d=\"M468.636377 745.435587c-11.622712 11.597129-18.006096 27.010167-18.006096 43.413765 0 16.404622 6.383384 31.816636 18.006096 43.439348 11.95324 11.927657 27.671223 17.904789 43.3626 17.904789 15.717983 0 31.434943-5.977132 43.388183-17.904789l0.025582-0.025582c11.597129-11.597129 17.981537-27.010167 17.981537-43.413766 0-16.404622-6.383384-31.816636-17.981537-43.388183-23.957645-23.957645-62.869886-23.933085-86.776365-0.025582z m49.949622 50.000787c-3.585665 3.585665-9.511631 3.63683-13.148462 0-2.364861-2.364861-2.720971-5.136997-2.720971-6.587022 0-1.424442 0.356111-4.196579 2.720971-6.561439a9.284457 9.284457 0 0 1 6.56144-2.720971c2.390443 0 4.780887 0.915859 6.612604 2.746553 2.339278 2.339278 2.695389 5.112438 2.695389 6.535857a9.312086 9.312086 0 0 1-2.720971 6.587022zM354.696348 631.546723l36.826743 36.826744c66.456574-66.430992 174.570921-66.430992 240.97633 0l36.826744-36.826744c-86.7252-86.726224-227.903593-86.776366-314.629817 0zM215.808114 492.657466l36.826744 36.826744c69.278853-69.278853 161.396877-107.454314 259.364119-107.454314 97.992825 0 190.112895 38.174438 259.391748 107.454314l36.826744-36.826744c-79.122035-79.122035-184.313819-122.713856-296.218492-122.713856-111.87909 0.001023-217.069851 43.592844-296.190863 122.713856zM76.893274 353.74365l36.826744 36.826743c219.612767-219.638349 576.946174-219.612767 796.559964 0l36.826744-36.826743c-239.883439-239.883439-630.27987-239.933581-870.213452 0z\"/>\n"\
"        </svg>\n"\
"        <input hidden disabled id=\"input\" name=\"SSID\" placeholder=\"请输入Wi-Fi名称\" type=\"text\">\n"\
"		<select id=\"select\" name=\"SSID\" id=\"select-value\"\">";

const char *index_html_end = 
"		</select>\n"\
"		<span id=\"s-icon\">\n"\
"			<svg class=\"icon yun-icon-right\" style=\"top: 6px;\" width=\"200px\" height=\"200.00px\" viewBox=\"0 0 1024 1024\" version=\"1.1\">\n"\
"				<path fill=\"#AeAeAe\" d=\"M503.466667 672l362.666666-362.666667c8.533333-8.533333 21.333333-8.533333 29.866667 0 8.533333 8.533333 8.533333 21.333333 0 29.866667L533.333333 704c-8.533333 8.533333-21.333333 8.533333-29.866666 0-8.533333-8.533333-8.533333-23.466667 0-32zM170.666667 311.466667l362.666666 362.666666c8.533333 8.533333 8.533333 21.333333 0 29.866667-8.533333 8.533333-21.333333 8.533333-29.866666 0l-362.666667-362.666667c-8.533333-8.533333-8.533333-21.333333 0-29.866666 6.4-8.533333 21.333333-8.533333 29.866667 0z\" />\n"\
"			</svg>\n"\
"		</span>\n"\
"    </div>\n"\
"    <div class=\"yun-row\">\n"\
"        <svg class=\"icon yun-icon-left yun-suo\" width=\"200px\" height=\"200.00px\" viewBox=\"0 0 1024 1024\" version=\"1.1\">\n"\
"            <path fill=\"#AeAeAe\" d=\"M896 341.333333h-106.666667V170.666667c0-93.866667-76.8-170.666667-170.666666-170.666667H405.333333c-93.866667 0-170.666667 76.8-170.666666 170.666667v170.666666H128c-36.266667 0-64 27.733333-64 64v554.666667c0 36.266667 27.733333 64 64 64h768c36.266667 0 64-27.733333 64-64V405.333333c0-36.266667-27.733333-64-64-64zM277.333333 192c0-83.2 64-149.333333 145.066667-149.333333h181.333333C682.666667 42.666667 746.666667 108.8 746.666667 192v149.333333H277.333333V192z m640 768c0 12.8-8.533333 21.333333-21.333333 21.333333H128c-12.8 0-21.333333-8.533333-21.333333-21.333333V405.333333c0-12.8 8.533333-21.333333 21.333333-21.333333h768c12.8 0 21.333333 8.533333 21.333333 21.333333v554.666667z\"/>\n"\
"            <path fill=\"#AeAeAe\" d=\"M499.2 578.133333c-25.6 4.266667-44.8 25.6-51.2 49.066667-6.4 34.133333 12.8 61.866667 40.533333 72.533333V768c0 12.8 8.533333 21.333333 21.333334 21.333333s21.333333-8.533333 21.333333-21.333333v-68.266667c25.6-8.533333 42.666667-32 42.666667-59.733333 2.133333-38.4-34.133333-70.4-74.666667-61.866667z\"/>\n"\
"        </svg>\n"\
"        <input name=\"PASS\" id=\"p-value\" placeholder=\"请输入Wi-Fi密码\" oninput=\"i()\" type=\"password\">\n"\
"		<span hidden id=\"p-icon\">\n"\
"			<svg onclick=\"showCode()\" class=\"icon yun-icon-right\" width=\"200px\" height=\"200.00px\" viewBox=\"0 0 1024 1024\" version=\"1.1\">\n"\
"			    <path fill=\"#AeAeAe\" d=\"M512 341.333333c-93.866667 0-170.666667 76.8-170.666667 170.666667s76.8 170.666667 170.666667 170.666667 170.666667-76.8 170.666667-170.666667-76.8-170.666667-170.666667-170.666667z m0 298.666667c-70.4 0-128-57.6-128-128s57.6-128 128-128 128 57.6 128 128-57.6 128-128 128z\"  /><path fill=\"#AeAeAe\" d=\"M512 149.333333C121.6 149.333333 0 512 0 512s134.4 362.666667 512 362.666667 512-362.666667 512-362.666667S902.4 149.333333 512 149.333333z m0 682.666667C206.933333 832 74.666667 576 46.933333 512 72.533333 448 198.4 192 512 192c315.733333 0 439.466667 253.866667 465.066667 320-27.733333 61.866667-162.133333 320-465.066667 320z\"  />\n"\
"			</svg>\n"\
"		</span>\n"\
"    </div>\n"\
"    <div class=\"yun-select\" style=\"padding: 6px 0;font-size: 12px;color: #0081F7;text-align: left\">\n"\
"        <span id=\"s-btn\" onclick=\"showSelect(false)\">列表中没有我的Wi-Fi，手动输入Wi-Fi名称</span>\n"\
"        <span id=\"i-btn\" onclick=\"showSelect(true)\" hidden>从列表中选择Wi-Fi名称</span>\n"\
"    </div>\n"\
"    <div class=\"yun-tip\" style=\"margin-top: 40px;padding: 16px 0;font-size: 12px;color: #888888\">目前暂不支持5G Wi-Fi</div>\n"\
"    <button type=\"submit\" value=\"submit\" class=\"yun-btn\">发送</button>\n"\
"</form>\n"\
"<script>\n"\
"    let password = document.getElementById('p-value');\n"\
"    let input = document.getElementById('input');\n"\
"    let select = document.getElementById('select');\n"\
"	function show (id) {\n"\
"		document.getElementById(id).removeAttribute('hidden')\n"\
"	}\n"\
"	function hid (id) {\n"\
"		document.getElementById(id).setAttribute('hidden', 'true')\n"\
"	}\n"\
"    function showSelect(s) {\n"\
"        if (s) {\n"\
"			show('s-btn');\n"\
"			show('select');\n"\
"			show('s-icon');\n"\
"            hid('i-btn');\n"\
"			hid('input');\n"\
"            document.getElementById('select').removeAttribute('disabled');\n"\
"            document.getElementById('input').setAttribute('disabled', 'true')\n"\
"        } else {\n"\
"            hid('s-btn');\n"\
"            hid('select');\n"\
"			hid('s-icon');\n"\
"            show('i-btn');\n"\
"            show('input');\n"\
"            document.getElementById('input').removeAttribute('disabled');\n"\
"            document.getElementById('select').setAttribute('disabled', 'true')\n"\
"        }\n"\
"    }\n"\
"	let ele = document.getElementById('p-value');\n"\
"    function showCode() {\n"\
"		password.getAttribute('type') === 'password' ? password.setAttribute('type', 'text') : password.setAttribute('type', 'password')\n"\
"    }\n"\
"	function i () {\n"\
"		password.value ? show('p-icon') : hid('p-icon')\n"\
"	}\n"\
"	function checkForm() {\n"\
"        console.log(password.value, input.value, select.value);\n"\
"	    if (input.hasAttribute('hidden') && !select.value) {\n"\
"	        alert('请选择WiFi！');\n"\
"	        return false\n"\
"        }\n"\
"        if (select.hasAttribute('hidden') && (!input.value || !input.value.trim())) {\n"\
"	        alert('请输入WiFi名称！');\n"\
"	        return false\n"\
"        }\n"\
"        if (!password.value || !password.value.trim()) {\n"\
"            alert('请输入WiFi密码！');\n"\
"            return false\n"\
"        }\n"\
"        return true\n"\
"    }\n"\
"</script>\n"\
"</body>\n"\
"</html>";
