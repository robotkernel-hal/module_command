from conans import tools, python_requires

base = python_requires("conan_template/[~=5]@robotkernel/stable")

class MainProject(base.RobotkernelConanFile):
    name = "module_command"
    description = "robotkernel-5 module to execute commands on trigger input."
    exports_sources = ["*", "!.gitignore"] + ["!%s" % x for x in tools.Git().excluded_files()]
    requires = "robotkernel/[~=5]@robotkernel/stable"

