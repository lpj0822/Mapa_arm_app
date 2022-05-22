apply s5l SDK file:
    inc:
        basetypes.h:ambarella/include/basetypes.h
	config.h:ambarella/boards/s5l_strawberry/config.h
	cnn/opt_sc_algos_interface.h:ambarella/prebuild/ambarella/library/ai_cam/amba_opt_sc_algos/include/opt_sc_algos_interface.h
	oryx/*:ambarella/oryx/*
	wissen/*: from alg wissen
	
    lib:
        from sdk ambarella/out/s5l_strawberry/fakeroot/usr/lib
	libSmart.so form alg wissen
