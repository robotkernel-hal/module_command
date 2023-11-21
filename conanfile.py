from conan import ConanFile

class MainProject(ConanFile):
    python_requires = "conan_template/[~=5]@robotkernel/stable"
    python_requires_extend = "conan_template.RobotkernelConanFile"

    name = "module_command"
    description = "robotkernel-5 module to execute commands on trigger input."
    exports_sources = ["*", "!.gitignore"]
    requires = "robotkernel/[~=5]@robotkernel/stable"

