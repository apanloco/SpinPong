rm *.elf
rm *.self
rm USRDIR/EBOOT.BIN
make
make_fself_npdrm *.elf EBOOT.BIN
mv EBOOT.BIN USRDIR/
rm *.pkg
make_package_npdrm
mv *.pkg SpinPongPs3.pkg
