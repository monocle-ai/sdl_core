"""JSONRPC XML parser unit test."""
import os
import sys
import unittest
from pathlib import Path

sys.path.append(Path(__file__).absolute().parents[3].as_posix())
sys.path.append(Path(__file__).absolute().parents[4].joinpath('rpc_spec/InterfaceParser').as_posix())
try:
    import generator.parsers.JSONRPC
    from model.array import Array
    from model.boolean import Boolean
    from model.enum_subset import EnumSubset
    from model.integer import Integer
    from model.float import Float
    from model.string import String
except ModuleNotFoundError as error:
    print('{}.\nJSONRPC\tProbably you did not initialize submodule'.format(error))
    sys.exit(1)


class TestJSONRPCVParser(unittest.TestCase):
    """Test for JSONRPC xml parser."""

    class _Issue:
        def __init__(self, creator, value):
            self.creator = creator
            self.value = value

        def __eq__(self, other):
            return self.creator == other.creator and self.value == other.value

    def setUp(self):
        """Test initialization."""
        self.valid_xml_name = os.path.dirname(os.path.realpath(__file__)) + \
                              "/valid_JSONRPC.xml"
        self.parser = generator.parsers.JSONRPC.Parser()

    def test_valid_xml(self):
        """Test parsing of valid xml."""
        interface = self.parser.parse(self.valid_xml_name)

        self.assertEqual(9, len(interface.params))
        self.assertDictEqual({"attr1": "v1",
                              "attr2": "v2",
                              "interface1_attribute1": "value1",
                              "interface1_attribute2": "value2",
                              "interface1_issues": "Issue1\nIssue2",
                              "interface1_design_description": "dd",
                              "interface2_attribute": "value",
                              "interface2_description":
                                  "Description of interface2",
                              "interface2_todos": "i2 todo"},
                             interface.params)

        # Enumerations

        self.assertEqual(5, len(interface.enums))

        # Enumeration "FunctionID"

        self.assertIn("FunctionID", interface.enums)
        enum = interface.enums["FunctionID"]
        self.verify_base_item(item=enum,
                              name="FunctionID")
        self.assertIsNone(enum.internal_scope)

        self.assertEqual(3, len(enum.elements))

        self.assertIn("interface1.Function1", enum.elements)
        element = enum.elements["interface1.Function1"]
        self.verify_base_item(
            item=element,
            name="interface1.Function1")
        self.assertEqual("interface1_Function1", element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("interface1.Function2", enum.elements)
        element = enum.elements["interface1.Function2"]
        self.verify_base_item(
            item=element,
            name="interface1.Function2")
        self.assertEqual("interface1_Function2", element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("interface2.Function1", enum.elements)
        element = enum.elements["interface2.Function1"]
        self.verify_base_item(
            item=element,
            name="interface2.Function1")
        self.assertEqual("interface2_Function1", element.internal_name)
        self.assertIsNone(element.value)

        # Enumeration "messageType"

        self.assertIn("messageType", interface.enums)
        enum = interface.enums["messageType"]
        self.verify_base_item(
            item=enum,
            name="messageType")
        self.assertIsNone(enum.internal_scope)

        self.assertEqual(3, len(enum.elements))

        self.assertIn("request", enum.elements)
        element = enum.elements["request"]
        self.verify_base_item(item=element,
                              name="request")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("response", enum.elements)
        element = enum.elements["response"]
        self.verify_base_item(item=element, name="response")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("notification", enum.elements)
        element = enum.elements["notification"]
        self.verify_base_item(item=element, name="notification")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        # Enumeration "interface1_enum1"

        self.assertIn("interface1_enum1", interface.enums)
        enum = interface.enums["interface1_enum1"]
        self.verify_base_item(item=enum, name="interface1_enum1",
                              platform="enum1 platform")
        self.assertEqual("scope", enum.internal_scope)

        self.assertEqual(3, len(enum.elements))

        self.assertIn("element1", enum.elements)
        element = enum.elements["element1"]
        self.verify_base_item(item=element, name="element1")
        self.assertIsNone(element.internal_name)
        self.assertEqual(10, element.value)

        self.assertIn("element2", enum.elements)
        element = enum.elements["element2"]
        self.verify_base_item(item=element, name="element2")
        self.assertEqual("element2_internal", element.internal_name)
        self.assertEqual(11, element.value)

        self.assertIn("element3", enum.elements)
        element = enum.elements["element3"]
        self.verify_base_item(
            item=element,
            name="element3",
            design_description=["Element design description"],
            platform="element3 platform")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        # Enumeration "interface2_enum1"

        self.assertIn("interface2_enum1", interface.enums)
        enum = interface.enums["interface2_enum1"]
        self.verify_base_item(item=enum, name="interface2_enum1",
                              description=["Interface2 enum1 description"])
        self.assertIsNone(enum.internal_scope)

        self.assertEqual(2, len(enum.elements))

        self.assertIn("e1", enum.elements)
        element = enum.elements["e1"]
        self.verify_base_item(item=element, name="e1")
        self.assertEqual("int_e1", element.internal_name)
        self.assertEqual(0, element.value)

        self.assertIn("e2", enum.elements)
        element = enum.elements["e2"]
        self.verify_base_item(item=element, name="e2")
        self.assertEqual("int_e2", element.internal_name)
        self.assertEqual(10, element.value)

        # Enumeration "interface2_enum2"

        self.assertIn("interface2_enum2", interface.enums)
        enum = interface.enums["interface2_enum2"]
        self.verify_base_item(item=enum, name="interface2_enum2",
                              platform="e2 platform")
        self.assertEqual("e2 scope", enum.internal_scope)

        self.assertEqual(3, len(enum.elements))

        self.assertIn("element1", enum.elements)
        element = enum.elements["element1"]
        self.verify_base_item(item=element, name="element1")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("element2", enum.elements)
        element = enum.elements["element2"]
        self.verify_base_item(item=element, name="element2")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        self.assertIn("element3", enum.elements)
        element = enum.elements["element3"]
        self.verify_base_item(item=element, name="element3")
        self.assertIsNone(element.internal_name)
        self.assertIsNone(element.value)

        # Structures

        self.assertEqual(3, len(interface.structs))

        # Structure "interface1_struct1"

        self.assertIn("interface1_struct1", interface.structs)
        struct = interface.structs["interface1_struct1"]
        self.verify_base_item(
            item=struct,
            name="interface1_struct1",
            description=["Struct description"],
            issues=[TestJSONRPCVParser._Issue(creator="creator1",
                                              value="Issue1"),
                    TestJSONRPCVParser._Issue(creator="creator2",
                                              value="Issue2")])

        self.assertEqual(4, len(struct.members))

        self.assertIn("member1", struct.members)
        member = struct.members["member1"]
        self.verify_base_item(
            item=member,
            name="member1",
            description=["Param1 description"])
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Integer)
        self.assertIsNone(member.param_type.min_value)
        self.assertIsNone(member.param_type.max_value)

        self.assertIn("member2", struct.members)
        member = struct.members["member2"]
        self.verify_base_item(item=member, name="member2",
                              platform="member2 platform")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Boolean)

        self.assertIn("member3", struct.members)
        member = struct.members["member3"]
        self.verify_base_item(item=member, name="member3")
        self.assertEqual(False, member.is_mandatory)
        self.assertIsInstance(member.param_type, Float)
        self.assertIsNone(member.param_type.min_value)
        self.assertAlmostEqual(20.5, member.param_type.max_value)

        self.assertIn("member4", struct.members)
        member = struct.members["member4"]
        self.verify_base_item(item=member, name="member4")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Array)
        self.assertIsNone(member.param_type.min_size)
        self.assertIsNone(member.param_type.max_size)
        self.assertIsInstance(member.param_type.element_type,
                              Integer)
        self.assertEqual(11, member.param_type.element_type.min_value)
        self.assertEqual(100, member.param_type.element_type.max_value)

        # Structure "interface1_struct2"

        self.assertIn("interface1_struct2", interface.structs)
        struct = interface.structs["interface1_struct2"]
        self.verify_base_item(item=struct,
                              name="interface1_struct2",
                              description=["Description of struct2"],
                              platform="struct2 platform")

        self.assertEqual(4, len(struct.members))

        self.assertIn("m1", struct.members)
        member = struct.members["m1"]
        self.verify_base_item(item=member, name="m1")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, String)
        self.assertIsNone(member.param_type.max_length)

        self.assertIn("m2", struct.members)
        member = struct.members["m2"]
        self.verify_base_item(item=member, name="m2")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Array)
        self.assertEqual(1, member.param_type.min_size)
        self.assertEqual(50, member.param_type.max_size)
        self.assertIsInstance(member.param_type.element_type,
                              String)
        self.assertEqual(100, member.param_type.element_type.max_length)

        self.assertIn("m3", struct.members)
        member = struct.members["m3"]
        self.verify_base_item(item=member, name="m3")
        self.assertTrue(member.is_mandatory)
        self.assertIs(member.param_type, interface.enums["interface1_enum1"])

        self.assertIn("m4", struct.members)
        member = struct.members["m4"]
        self.verify_base_item(item=member, name="m4")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Array)
        self.assertIsNone(member.param_type.min_size)
        self.assertEqual(10, member.param_type.max_size)
        self.assertIs(member.param_type.element_type,
                      interface.structs["interface1_struct1"])

        # Structure "interface2_struct1"

        self.assertIn("interface2_struct1", interface.structs)
        struct = interface.structs["interface2_struct1"]
        self.verify_base_item(item=struct, name="interface2_struct1")

        self.assertEqual(3, len(struct.members))

        self.assertIn("m_1", struct.members)
        member = struct.members["m_1"]
        self.verify_base_item(item=member, name="m_1")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, Array)
        self.assertEqual(1, member.param_type.min_size)
        self.assertEqual(10, member.param_type.max_size)
        self.assertIs(member.param_type.element_type,
                      interface.enums["interface2_enum1"])

        self.assertIn("m_2", struct.members)
        member = struct.members["m_2"]
        self.verify_base_item(item=member, name="m_2")
        self.assertTrue(member.is_mandatory)
        self.assertIs(member.param_type, interface.enums["interface2_enum2"])

        self.assertIn("m_3", struct.members)
        member = struct.members["m_3"]
        self.verify_base_item(item=member, name="m_3")
        self.assertTrue(member.is_mandatory)
        self.assertIsInstance(member.param_type, String)
        self.assertEqual(20, member.param_type.max_length)

        # Functions

        self.assertEqual(5, len(interface.functions))

        # Function request "interface1_Function1"

        self.assertIn(
            (interface.enums["FunctionID"].elements["interface1.Function1"],
             interface.enums["messageType"].elements["request"]),
            interface.functions)
        function = interface.functions[
            (interface.enums["FunctionID"].elements["interface1.Function1"],
             interface.enums["messageType"].elements["request"])]
        self.verify_base_item(
            item=function,
            name="interface1_Function1",
            description=["Description of request Function1"],
            todos=["Function1 request todo"])
        self.assertIs(
            function.function_id,
            interface.enums["FunctionID"].elements["interface1.Function1"])
        self.assertIs(function.message_type,
                      interface.enums["messageType"].elements["request"])

        self.assertEqual(3, len(function.params))

        self.assertIn("param1", function.params)
        param = function.params["param1"]
        self.verify_base_item(
            item=param,
            name="param1",
            issues=[TestJSONRPCVParser._Issue(creator="", value="")])
        self.assertEqual(False, param.is_mandatory)
        self.assertIsInstance(param.param_type, String)
        self.assertIsNone(param.param_type.max_length)
        self.assertIsNone(param.default_value)
        self.assertEqual("String default value", param.param_type.default_value)

        self.assertIn("param2", function.params)
        param = function.params["param2"]
        self.verify_base_item(
            item=param,
            name="param2",
            description=["Param2 description", ""],
            todos=["Param2 todo"],
            platform="param2 platform")
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, Integer)
        self.assertIsNone(param.param_type.min_value)
        self.assertIsNone(param.param_type.max_value)
        self.assertIsNone(param.default_value)

        self.assertIn("param3", function.params)
        param = function.params["param3"]
        self.verify_base_item(item=param, name="param3")
        self.assertEqual(False, param.is_mandatory)
        self.assertIs(param.param_type,
                      interface.structs["interface1_struct1"])
        self.assertIsNone(param.default_value)

        # Function response "interface1_Function1"

        self.assertIn(
            (interface.enums["FunctionID"].elements["interface1.Function1"],
             interface.enums["messageType"].elements["response"]),
            interface.functions)
        function = interface.functions[
            (interface.enums["FunctionID"].elements["interface1.Function1"],
             interface.enums["messageType"].elements["response"])]
        self.verify_base_item(
            item=function,
            name="interface1_Function1",
            issues=[TestJSONRPCVParser._Issue(creator="c1", value=""),
                    TestJSONRPCVParser._Issue(creator="c2", value="")],
            platform="")
        self.assertIs(
            function.function_id,
            interface.enums["FunctionID"].elements["interface1.Function1"])
        self.assertIs(function.message_type,
                      interface.enums["messageType"].elements["response"])

        self.assertEqual(3, len(function.params))

        self.assertIn("p1", function.params)
        param = function.params["p1"]
        self.verify_base_item(item=param, name="p1")
        self.assertTrue(param.is_mandatory)
        self.assertIs(param.param_type, interface.enums["interface1_enum1"])
        self.assertIsNone(param.default_value)

        self.assertIn("p2", function.params)
        param = function.params["p2"]
        self.verify_base_item(item=param, name="p2")
        self.assertTrue(param.is_mandatory)
        self.assertIs(param.param_type, interface.enums["interface1_enum1"])
        self.assertIsNone(param.default_value)
        self.assertIsNone(param.param_type.default_value)
        self.assertIs(param.param_type.elements["element2"],
                      interface.enums["interface1_enum1"].elements["element2"])

        self.assertIn("p3", function.params)
        param = function.params["p3"]
        self.verify_base_item(item=param, name="p3", design_description=[""])
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, Boolean)
        self.assertIsNone(param.default_value)
        self.assertFalse(param.param_type.default_value)

        # Function notification "interface1_Function2"

        self.assertIn(
            (interface.enums["FunctionID"].elements["interface1.Function2"],
             interface.enums["messageType"].elements["notification"]),
            interface.functions)
        function = interface.functions[
            (interface.enums["FunctionID"].elements["interface1.Function2"],
             interface.enums["messageType"].elements["notification"])]
        self.verify_base_item(item=function,
                              name="interface1_Function2",
                              description=["Function2 description"],
                              platform="function2 platform")
        self.assertIs(
            function.function_id,
            interface.enums["FunctionID"].elements["interface1.Function2"])
        self.assertIs(function.message_type,
                      interface.enums["messageType"].elements["notification"])

        self.assertEqual(3, len(function.params))

        self.assertIn("n1", function.params)
        param = function.params["n1"]
        self.verify_base_item(item=param, name="n1", todos=["n1 todo"])
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, EnumSubset)
        self.assertIs(param.param_type.enum,
                      interface.enums["interface1_enum1"])
        self.assertDictEqual(
            {"element2":
                 interface.enums["interface1_enum1"].elements["element2"],
             "element3":
                 interface.enums["interface1_enum1"].elements["element3"]},
            param.param_type.allowed_elements)
        self.assertIsNone(param.default_value)

        self.assertIn("n2", function.params)
        param = function.params["n2"]
        self.verify_base_item(item=param, name="n2", todos=["n2 todo"])
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, Array)
        self.assertEqual(1, param.param_type.min_size)
        self.assertEqual(100, param.param_type.max_size)
        self.assertIsInstance(param.param_type.element_type,
                              EnumSubset)
        self.assertIs(param.param_type.element_type.enum,
                      interface.enums["interface1_enum1"])
        self.assertDictEqual(
            {"element1":
                 interface.enums["interface1_enum1"].elements["element1"],
             "element3":
                 interface.enums["interface1_enum1"].elements["element3"]},
            param.param_type.element_type.allowed_elements)
        self.assertIsNone(param.default_value)

        self.assertIn("n3", function.params)
        param = function.params["n3"]
        self.verify_base_item(item=param, name="n3")
        self.assertEqual(False, param.is_mandatory)
        self.assertIs(param.param_type,
                      interface.structs["interface1_struct2"])
        self.assertIsNone(param.default_value)

        # Function request "interface2_Function1"

        self.assertIn(
            (interface.enums["FunctionID"].elements["interface2.Function1"],
             interface.enums["messageType"].elements["request"]),
            interface.functions)
        function = interface.functions[
            (interface.enums["FunctionID"].elements["interface2.Function1"],
             interface.enums["messageType"].elements["request"])]
        self.verify_base_item(item=function, name="interface2_Function1")
        self.assertIs(
            function.function_id,
            interface.enums["FunctionID"].elements["interface2.Function1"])
        self.assertIs(function.message_type,
                      interface.enums["messageType"].elements["request"])

        self.assertEqual(2, len(function.params))

        self.assertIn("param1", function.params)
        param = function.params["param1"]
        self.verify_base_item(item=param, name="param1")
        self.assertEqual(False, param.is_mandatory)
        self.assertIs(param.param_type, interface.enums["interface2_enum1"])
        self.assertIsNone(param.default_value)

        self.assertIn("param2", function.params)
        param = function.params["param2"]
        self.verify_base_item(item=param, name="param2")
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, Array)
        self.assertEqual(5, param.param_type.min_size)
        self.assertEqual(25, param.param_type.max_size)
        self.assertIs(param.param_type.element_type,
                      interface.structs["interface2_struct1"])
        self.assertIsNone(param.default_value)

        # Function notification "interface2_Function1"

        self.assertIn(
            (interface.enums["FunctionID"].elements["interface2.Function1"],
             interface.enums["messageType"].elements["notification"]),
            interface.functions)
        function = interface.functions[
            (interface.enums["FunctionID"].elements["interface2.Function1"],
             interface.enums["messageType"].elements["notification"])]
        self.verify_base_item(
            item=function,
            name="interface2_Function1",
            issues=[
                TestJSONRPCVParser._Issue(creator="c", value="Issue text")],
            platform="platform")
        self.assertIs(
            function.function_id,
            interface.enums["FunctionID"].elements["interface2.Function1"])
        self.assertIs(function.message_type,
                      interface.enums["messageType"].elements["notification"])

        self.assertEqual(2, len(function.params))

        self.assertIn("param", function.params)
        param = function.params["param"]
        self.verify_base_item(item=param, name="param")
        self.assertTrue(param.is_mandatory)
        self.assertIsInstance(param.param_type, EnumSubset)
        self.assertIs(param.param_type.enum,
                      interface.enums["interface2_enum2"])
        self.assertDictEqual(
            {"element2":
                 interface.enums["interface2_enum2"].elements["element2"],
             "element3":
                 interface.enums["interface2_enum2"].elements["element3"]},
            param.param_type.allowed_elements)
        self.assertIsNone(param.default_value)

        self.assertIn("i1", function.params)
        param = function.params["i1"]
        self.verify_base_item(item=param, name="i1")
        self.assertTrue(param.is_mandatory)
        self.assertIs(param.param_type,
                      interface.structs["interface1_struct2"])
        self.assertIsNone(param.default_value)

    def verify_base_item(self, item, name, description=None,
                         design_description=None, issues=None, todos=None,
                         platform=None):
        """Verify base interface item variables."""
        self.assertEqual(name, item.name)
        self.assertSequenceEqual(self.get_list(description), item.description)
        self.assertSequenceEqual(self.get_list(design_description),
                                 item.design_description)
        self.assertSequenceEqual(self.get_list(issues), item.issues)
        self.assertSequenceEqual(self.get_list(todos), item.todos)
        self.assertEqual(platform, item.platform)

    @staticmethod
    def get_list(list=None):
        """Return provided list or empty list if None is provided."""
        return list if list is not None else []


if __name__ == "__main__":
    unittest.main()
