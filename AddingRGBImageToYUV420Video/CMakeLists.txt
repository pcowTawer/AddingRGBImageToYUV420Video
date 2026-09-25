# CMakeList.txt: проект CMake для AddingRGBImageToYUV420Video; включите исходный код и определения,
# укажите здесь логику для конкретного проекта.
#

# Добавьте источник в исполняемый файл этого проекта.
add_executable (AddingRGBImageToYUV420Video "AddingRGBImageToYUV420Video.cpp" "AddingRGBImageToYUV420Video.h")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET AddingRGBImageToYUV420Video PROPERTY CXX_STANDARD 11)
endif()

# TODO: Добавьте тесты и целевые объекты, если это необходимо.
