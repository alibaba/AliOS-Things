文件说明
---

	文件名 | 说明
	-------|-----
	section.$i.$j | 输入文件, 记录 make.settings 中的开关段落
	make.settings.sample.$x.$y.$z | 输出文件, 组合 make.settings 的开关段落到完整配置
	enum_config.sh | 工作脚本, 从输入文件产生输出文件

使用方法
---
	for iter in doc/make.settings.sections/make.settings.sample.*; do
		cp ${iter} make.settings
		make
	done

