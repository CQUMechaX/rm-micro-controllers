/home/mechax/.platformio/packages/tool-openocd/bin/openocd -d2 -s /home/mechax/.platformio/packages/tool-openocd/scripts -f interface/stlink.cfg -c "transport select hla_swd" -f target/stm32f4x.cfg -c "program {USB.axf}  verify reset; shutdown;"
