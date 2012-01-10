project(Python)

file(GLOB_RECURSE modules "*.py")

python_install_modules(${modules})
