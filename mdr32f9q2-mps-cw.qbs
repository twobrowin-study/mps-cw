import qbs
Project {
  name: "mdr32f9q2-mps-cw"
  property string stdPerLibPath: "/lib/mdr32f9q2-std-per-lib/"
  property string startupPath: stdPerLibPath + "CMSIS/CM3/DeviceSupport/MDR32F9Qx/startup/"
  property string perfLibsSrcPath: stdPerLibPath + "MDR32F9Qx_StdPeriph_Driver/src/"
  property string lnFile: startupPath + "gcc/MDR32F9Qx.ld"
  property string userLibPath: "UserLib/"
  property string userLibDiagnosticPath: userLibPath + "Diagnostic/"
  property string userLibDiagnosticPathSrcPath: userLibDiagnosticPath + "src/"
  property string userLibPathSrcPath: userLibPath + "src/"
  property string lcdLibPath: "LCDLib/"
  property string lcdLibPathsSrcPath: lcdLibPath + "src/"
  property string lcdLibPathsFontsPath: lcdLibPath + "fonts/"
  property string sensorsCount: "6" // Количество датчиков в МК-сети
  property bool diagnostic: true // Включение режима диагностики в сборку
  property pathList includePaths: [
    stdPerLibPath,
    stdPerLibPath + "CMSIS/CM3/CoreSupport/",
    stdPerLibPath + "CMSIS/CM3/DeviceSupport/MDR32F9Qx/inc/",
    startupPath,
    stdPerLibPath + "Config/",
    stdPerLibPath + "MDR32F9Qx_StdPeriph_Driver/inc/",
    stdPerLibPath + "MDR32F9Qx_StdPeriph_Driver/inc/USB_Library/",
    userLibPath + "inc/",
    userLibDiagnosticPath + "inc/",
    lcdLibPath + "inc/",
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
        project.userLibPathSrcPath + "delay.c"
      ]
      fileTags: ['ul']
    }
    Group {
      name: "user-lib-diagnostic"
      files: [
        project.userLibDiagnosticPathSrcPath + "diagnostic.c",
        project.userLibDiagnosticPathSrcPath + "cp1251_diagnostic_menu.c",
        project.userLibDiagnosticPathSrcPath + "diagnostic_start.c"
      ]
      fileTags: ['uld']
    }
    Group {
      name: "perf-lib"
      files: [
        project.perfLibsSrcPath + "MDR32F9Qx_rst_clk.c",
        project.perfLibsSrcPath + "MDR32F9Qx_port.c",
        project.perfLibsSrcPath + "MDR32F9Qx_timer.c"
      ]
      fileTags: ['pl']
    }
    Group {
      name: "lcd-lib"
      files: [
        project.lcdLibPathsSrcPath + "gl.c",
        project.lcdLibPathsSrcPath + "joystick.c",
        project.lcdLibPathsSrcPath + "lcd.c",
        project.lcdLibPathsSrcPath + "leds.c",
        project.lcdLibPathsSrcPath + "menu.c",
        project.lcdLibPathsSrcPath + "menu_gl.c",
        project.lcdLibPathsSrcPath + "menu_leds.c",
        project.lcdLibPathsSrcPath + "menu_technology.c",
        project.lcdLibPathsSrcPath + "menu_text.c",
        project.lcdLibPathsSrcPath + "systick.c",
        project.lcdLibPathsSrcPath + "text.c",
        project.lcdLibPathsFontsPath + "font_6x8.c",
        project.lcdLibPathsFontsPath + "font_7x10_bold.c",
        project.lcdLibPathsFontsPath + "font_7x10_thin.c",
        project.lcdLibPathsFontsPath + "font_12x16.c"
      ]
      fileTags: ['ll']
    }
    Rule {
      inputs: ['s', 'ul', 'uld', 'sl', 'pl', 'll']
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
