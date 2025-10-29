# GeminiCallCpp
A cpp program that asks the user for a prompt and then runs it through gemini to generate a response.

This program allows the integration of gemini into cpp.
Steps to run this program:
Make sure to configure vcpkg -> use these two commands in the same folder as CMakeList.txt
>vcpkg new --application

>vcpkg add port [name of package/library]

Please create a config.cpp file to store the Gemini API key.

I understand this isn't the most secure method, but this is intended as a homemade project. I'm aware
that alternatives like environment variables exist, but for the sake of simplicity, I'm using a config.cpp implementation.
