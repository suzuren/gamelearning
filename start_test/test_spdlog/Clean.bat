@echo Off

color 0a

pushd %~dp0..\

echo %cd% 
del /s /a *.suo *.ncb *.user *.pdb *.netmodule *.aps *.ilk *.bak *.sdf *.old2 *.sdf>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\Bin" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\Obj" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\ipch" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\Debug" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\Release" 2>nul

FOR /R . %%d IN (.) DO rd /s /q "%%d\.vs" 2>nul
FOR /R . %%d IN (.) DO rd /s /q "%%d\logs" 2>nul

popd
pause

