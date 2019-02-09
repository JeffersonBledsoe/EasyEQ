pushd ../JUCE/extras/Projucer/Builds/MacOSX
xcodebuild -configuration "Release" GCC_PREPROCESSOR_DEFINITIONS='$(inherited) JUCER_ENABLE_GPL_MODE'
popd
exec ../JUCE/extras/Projucer/Builds/MacOSX/build/Release/Projucer.app/Contents/MacOS/Projucer ./*.jucer & exit