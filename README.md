# LetMeGG

## What's this?
A POC about how to prevent windbg break

## Principle
```
int2D(ExceptionInformation[0]=5)->KiDebugServiceTrap->KiExceptionDispatch->KiDispatchException->KdTrap->KdpTrap/KdpStub->KdpCommandString
```

![image](https://user-images.githubusercontent.com/13917777/193211584-78db02a6-3912-44c2-ab35-509a0b16f4b1.png)

## Compile
- Visual Studio 2022
- llvm-msvc [[link]](https://github.com/NewWorldComingSoon/llvm-msvc-build)
