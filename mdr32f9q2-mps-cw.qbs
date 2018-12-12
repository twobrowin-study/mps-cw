import qbs
import qbs.Environment

Project {
  name: "mdr32f9q2-mps-cw"
  property string home: Environment.getEnv("HOME")
  property string stdPerLibPath: home + "/.local/include/mdr32f9q2-std-per-lib/"
  property string startupPath: stdPerLibPath + "CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/"
  property string perfLibsSrcPath: stdPerLibPath + "MDR32F9Qx_StdPeriph_Driver/src/"
  property string lnFile: startupPath + "gcc/MDR32F9Qx.ld"
  property string userLibPath: "UserLib/"
  property string userLibDiagnosticPath: userLibPath + "diagnostic/"
  property string userLibDiagnosticPathSrcPath: userLibDiagnosticPath + "src/"
  property string userLibPathSrcPath: userLibPath + "src/"
  property string lcdLibPath: "LCDLib/"
  property string lcdLibPathsSrcPath: lcdLibPath + "src/"
  property string usbLibPath: "USBLib/"
  property string usbLibPathsSrcPath: usbLibPath + "src/"
  property string lcdLibPathsFontsPath: lcdLibPath + "fonts/"
  property string sensorsCount: "4" // Количество датчиков в МК-сети
  property string sensorsBufLength: "32" // Размер буфера данных датчиков
  property bool diagnostic: true // Включение режима диагностики в сборку
  property pathList includePaths: [
    stdPerLibPath,
    stdPerLibPath + "CMSIS/CM3/CoreSupport/",
    stdPerLibPath + "CMSIS/CM3/DeviceSupport/MDR32F9Qx/inc/",
    startupPath,
    stdPerLibPath + "Config/",
    stdPerLibPath + "MDR32F9Qx_StdPeriph_Driver/inc/",
    userLibPath + "inc/",
    userLibDiagnosticPath + "inc/",
    lcdLibPath + "inc/",
    usbLibPath + "inc/",
    "./"
  ]
  Product {
    name: "mdr32f9q2-micro"
    type: "elf"
    Group {
      name: "sources"
      files: [
        "main.c"
      ]
      fileTags: ['s']
    }
    Group {
      name: "startup-lib"
      files: [
        project.startupPath + "system_MDR32F9Qx.c",
        project.startupPath + "gcc/startup_MDR32F9Qx.S"
      ]
      fileTags: ['sl']
    }
    Group {
      name: "user-lib"
      files: [
        project.userLibPathSrcPath + "delay.c",
        project.userLibPathSrcPath + "time.c",
        project.userLibPathSrcPath + "format.c",
        project.userLibPathSrcPath + "usb.c",
        project.userLibPathSrcPath + "can.c"
      ]
      fileTags: ['ul']
    }
    Group {
      name: "user-lib-diagnostic"
      files: [
        project.userLibDiagnosticPathSrcPath + "diagnostic.c",
        project.userLibDiagnosticPathSrcPath + "cp1251_diagnostic_menu.c",
        project.userLibDiagnosticPathSrcPath + "cp1251_diagnostic_menu_items.c",
        project.userLibDiagnosticPathSrcPath + "diagnostic_start.c"
      ]
      fileTags: ['uld']
    }
    Group {
      name: "perf-lib"
      files: [
        project.perfLibsSrcPath + "MDR32F9Qx_rst_clk.c",
        project.perfLibsSrcPath + "MDR32F9Qx_port.c",
        project.perfLibsSrcPath + "MDR32F9Qx_timer.c",
        project.perfLibsSrcPath + "MDR32F9Qx_bkp.c",
        project.perfLibsSrcPath + "MDR32F9Qx_can.c",
        project.perfLibsSrcPath + "MDR32F9Qx_eeprom.c",
        project.perfLibsSrcPath + "MDR32F9Qx_power.c"
      ]
      fileTags: ['pl']
    }
    Group {
      name: "lcd-lib"
      files: [
        project.lcdLibPathsSrcPath + "gl.c",
        project.lcdLibPathsSrcPath + "joystick.c",
        project.lcdLibPathsSrcPath + "lcd.c",
        project.lcdLibPathsSrcPath + "systick.c",
        project.lcdLibPathsSrcPath + "text.c",
        project.lcdLibPathsFontsPath + "font_6x8.c"
      ]
      fileTags: ['ll']
    }
    Group {
      name: "usb-lib"
      files: [
        project.usbLibPathsSrcPath + "mdr_usb_cdc.c"
      ]
      fileTags: ['uul']
    }
    Rule {
      inputs: ['s', 'ul', 'uld', 'sl', 'pl', 'll', 'uul']
      Artifact {
        fileTags: ['obj']
        filePath: input.fileName + '.o'
      }
      prepare: {
        var args = []
        args.push("-mcpu=cortex-m3")
        args.push("-mthumb")
        args.push("-g")
        args.push("-ffunction-sections")
        args.push("-O0")
        args.push("-Wall")
        args.push("-Wunused")
        args.push("-DM3")
        args.push("-nostartfiles")
        args.push("-D__START=main") // Указание исполняемую после начала функцию
        args.push("-D__CMCARM_DEMO__") // Указание для библиотеки LCDLib
        args.push("-DSENSORS_COUNT=" + project.sensorsCount) // Указание количества датчиков в сети
        args.push("-DSENSORS_BUF_LENGTH=" + project.sensorsBufLength) // Указание длины буфера датчиков
        for(i in project.includePaths)
          args.push("-I" + project.includePaths[i])
        args.push("-c")
        args.push(input.filePath)
        args.push("-o")
        args.push(output.filePath);
        var compilerPath = "arm-none-eabi-gcc"
        var cmd = new Command(compilerPath, args)
        cmd.description = "compiling " + input.fileName
        cmd.highlight = "compiler"
        cmd.silent = false
        return cmd
      }
    }
    Rule{
      multiplex: true
      inputs: ['obj']
      Artifact{
        fileTags:['elf']
        filePath: project.name + '.elf'
      }
      prepare:{
        var args = []
        args.push("-mcpu=cortex-m3")
        args.push("-mthumb")
        args.push("-g")
        args.push("-nostartfiles")
        args.push("-O0")
        args.push("-Wl,--gc-sections")
        for(i in inputs['obj'])
          args.push(inputs['obj'][i].filePath)
        args.push("-T" + project.lnFile)
        args.push("-o")
        args.push(output.filePath)
        var compilerPath = "arm-none-eabi-gcc"
        var cmd = new Command(compilerPath, args)
        cmd.description = "linking " + project.name
        return cmd
      }
    }
  }
}
