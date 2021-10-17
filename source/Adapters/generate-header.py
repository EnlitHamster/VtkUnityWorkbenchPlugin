import os

adapter_utility_h_name = "vtkAdapterUtility.h"
adapter_utility_cpp_name = "vtkAdapterUtility.cpp"
adapter_utility_class_name = "VtkAdapterUtility"
adapter_utility_getter_name = "GetAdapter"
adapter_utility_register_name = "s_adapters"
adapter_utility_register_type = "const std::unordered_map<LPCSTR, VtkAdapter*>"

adapter_utility_h = open(adapter_utility_h_name, "w")
adapter_utility_cpp = open(adapter_utility_cpp_name, "w")



### GENERATING HEADER FILE

adapter_utility_h.write("#pragma once\n")
adapter_utility_h.write("\n")
adapter_utility_h.write("/* This file has been automatically generated through the Python header generation utility\n")
adapter_utility_h.write(" * \n")
adapter_utility_h.write(" * This file contains the necessary information to allow the VtkToUnity plugin to know\n")
adapter_utility_h.write(" * that the adapters exist and it can call them. As such, this should be generated every\n")
adapter_utility_h.write(" * time the plugin is built to avoid losing any adapters in the compilation.\n")
adapter_utility_h.write(" */\n")
adapter_utility_h.write("\n")
adapter_utility_h.write("\n")

adapter_utility_h.write("// Utility includes\n")
adapter_utility_h.write("#include <unordered_map>\n")
adapter_utility_h.write("\n")
adapter_utility_h.write("#define NOMINMAX\n")
adapter_utility_h.write("#include <windows.h>\n")
adapter_utility_h.write("\n")
adapter_utility_h.write("#include \"Singleton.h\"\n")
adapter_utility_h.write("#include \"vtkAdapter.h\"\n")
adapter_utility_h.write("\n")


adapter_utility_h.write("\n")
adapter_utility_h.write("\n")
adapter_utility_h.write("// This class is used to register the adapters\n")
adapter_utility_h.write(f"class {adapter_utility_class_name}\n")
adapter_utility_h.write("{\n")

# Generating the class for the utility operations

adapter_utility_h.write("public:\n")

# begin public area

adapter_utility_h.write(f"\tstatic VtkAdapter* {adapter_utility_getter_name}(\n")
adapter_utility_h.write("\t\tLPCSTR vtkAdaptedObject);\n")
adapter_utility_h.write("\n")

# end public area

adapter_utility_h.write("private:\n")

# begin private area

adapter_utility_h.write("\t// Map with all the adapters registered in this folder\n")
adapter_utility_h.write(f"\tstatic {adapter_utility_register_type} {adapter_utility_register_name};\n")

# end private area

adapter_utility_h.write("};\n")



### GENERATING SOURCE FILE

adapter_utility_cpp.write("/* This file has been automatically generated through the Python header generation utility\n")
adapter_utility_cpp.write(" * \n")
adapter_utility_cpp.write(" * This file contains the necessary information to allow the VtkToUnity plugin to know\n")
adapter_utility_cpp.write(" * that the adapters exist and it can call them. As such, this should be generated every\n")
adapter_utility_cpp.write(" * time the plugin is built to avoid losing any adapters in the compilation.\n")
adapter_utility_cpp.write(" */\n")
adapter_utility_cpp.write("\n")
adapter_utility_cpp.write("\n")
adapter_utility_cpp.write(f"#include \"{adapter_utility_h_name}\"\n")
adapter_utility_cpp.write("\n")
adapter_utility_cpp.write("// Adapters' header files found in the folder (.h and .hpp)\n")

# Generating the includes of the adapters' header files

classes = []

for f in os.listdir("."):
    if f != adapter_utility_h_name and ( f.endswith(".h") or f.endswith(".hpp") ):
        adapter_utility_cpp.write(f"#include \"{f}\"\n")
        class_name = os.path.splitext(f)[0]
        classes.append(class_name[:1].upper() + class_name[1:])

adapter_utility_cpp.write("\n")
adapter_utility_cpp.write("\n")

# Creating the adapters' mapping

adapter_utility_cpp.write(f"{adapter_utility_register_type} {adapter_utility_class_name}::{adapter_utility_register_name} =");
adapter_utility_cpp.write("{\n")

# begin s_adapters init

for clss in classes:    
    adapter_utility_cpp.write(f"\t{{ Singleton<{clss}>::Instance()->GetAdaptingObject(), Singleton<{clss}>::Instance() }},\n")

# end s_adapters init

adapter_utility_cpp.write("};\n")

adapter_utility_cpp.write("\n")
adapter_utility_cpp.write("\n")

adapter_utility_cpp.write(f"VtkAdapter* {adapter_utility_class_name}::{adapter_utility_getter_name}(\n")
adapter_utility_cpp.write("\tLPCSTR vtkAdaptedObject)\n")
adapter_utility_cpp.write("{\n")

# begin GetAdapter

adapter_utility_cpp.write("\tauto itAdapter = s_adapters.find(vtkAdaptedObject);\n")
adapter_utility_cpp.write("\tif (itAdapter != s_adapters.end())\n")
adapter_utility_cpp.write("\t{\n")
adapter_utility_cpp.write("\t\treturn itAdapter->second;\n")
adapter_utility_cpp.write("\t}\n")
adapter_utility_cpp.write("\telse\n")
adapter_utility_cpp.write("\t{\n")
adapter_utility_cpp.write("\t\treturn NULL;\n")
adapter_utility_cpp.write("\t}\n")

# end GetAdapter

adapter_utility_cpp.write("}\n")