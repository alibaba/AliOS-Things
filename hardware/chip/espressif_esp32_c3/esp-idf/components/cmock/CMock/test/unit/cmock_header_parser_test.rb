# ==========================================
#   CMock Project - Automatic Mock Generation for C
#   Copyright (c) 2007 Mike Karlesky, Mark VanderVoord, Greg Williams
#   [Released under MIT License. Please refer to license.txt for details]
# ==========================================

$ThisIsOnlyATest = true

require File.expand_path(File.dirname(__FILE__)) + "/../test_helper"
require File.expand_path(File.dirname(__FILE__)) + '/../../lib/cmock_header_parser'

describe CMockHeaderParser, "Verify CMockHeaderParser Module" do

  before do
    create_mocks :config
    @test_name = 'test_file.h'
    @config.expect :strippables, ["STRIPPABLE"]
    @config.expect :attributes, ['__ramfunc', 'funky_attrib', 'SQLITE_API']
    @config.expect :c_calling_conventions, ['__stdcall']
    @config.expect :treat_as_void, ['MY_FUNKY_VOID']
    @config.expect :treat_as, { "BANJOS" => "INT", "TUBAS" => "HEX16"}
    @config.expect :treat_as_array, {"IntArray" => "int", "Book" => "Page"}
    @config.expect :when_no_prototypes, :error
    @config.expect :verbosity, 1
    @config.expect :treat_externs, :exclude
    @config.expect :treat_inlines, :exclude
    @config.expect :inline_function_patterns, ['(static\s+inline|inline\s+static)\s*', '(\bstatic\b|\binline\b)\s*']
    @config.expect :array_size_type, ['int', 'size_t']
    @config.expect :array_size_name, 'size|len'

    @parser = CMockHeaderParser.new(@config)
  end

  after do
  end

  it "create and initialize variables to defaults appropriately" do
    assert_equal([], @parser.funcs)
    assert_equal(['const', '__ramfunc', 'funky_attrib', 'SQLITE_API'], @parser.c_attributes)
    assert_equal(['void','MY_FUNKY_VOID'], @parser.treat_as_void)
  end

  it "strip out line comments" do
    source =
      " abcd;\n" +
      "// hello;\n" +
      "who // is you\n"

    expected =
    [
      "abcd",
      "who"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove block comments" do
    source =
      " no_comments;\n" +
      "// basic_line_comment;\n" +
      "/* basic_block_comment;*/\n" +
      "pre_block; /* start_of_block_comment;\n" +
      "// embedded_line_comment_in_block_comment; */\n" +
      "// /* commented_out_block_comment_line\n" +
      "shown_because_block_comment_invalid_from_line_comment;\n" +
      "// */\n" +
      "//* shorter_commented_out_block_comment_line; \n" +
      "shown_because_block_comment_invalid_from_shorter_line_comment;\n" +
      "/*/\n" +
      "not_shown_because_line_above_started_comment;\n" +
      "//*/\n" +
      "/* \n" +
      "not_shown_because_block_comment_started_this_time;\n" +
      "/*/\n" +
      "shown_because_line_above_ended_comment_this_time;\n" +
      "//*/\n"

    expected =
    [
      "no_comments",
      "pre_block",
      "shown_because_block_comment_invalid_from_line_comment",
      "shown_because_block_comment_invalid_from_shorter_line_comment",
      "shown_because_line_above_ended_comment_this_time"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove strippables from the beginning or end of function declarations" do
    source =
      "void* my_calloc(size_t, size_t) STRIPPABLE;\n" +
      "void\n" +
      "  my_realloc(void*, size_t) STRIPPABLE;\n" +
      "extern int\n" +
      "  my_printf (void *my_object, const char *my_format, ...)\n" +
      "  STRIPPABLE;\n" +
      "  void STRIPPABLE universal_handler ();\n"

    expected =
    [
      "void* my_calloc(size_t, size_t)",
      "void my_realloc(void*, size_t)",
      "void universal_handler()"
    ]

    assert_equal(expected, @parser.import_source(source))
  end

  it "remove gcc's function __attribute__'s" do
    source =
      "void* my_calloc(size_t, size_t) __attribute__((alloc_size(1,2)));\n" +
      "void\n" +
      "  my_realloc(void*, size_t) __attribute__((alloc_size(2)));\n" +
      "extern int\n" +
      "  my_printf (void *my_object, const char *my_format, ...)\n" +
      "  __attribute__ ((format (printf, 2, 3)));\n" +
      "  void __attribute__ ((interrupt)) universal_handler ();\n"

    expected =
    [
      "void* my_calloc(size_t, size_t)",
      "void my_realloc(void*, size_t)",
      "void universal_handler()"
    ]

    assert_equal(expected, @parser.import_source(source))
  end

  it "remove preprocessor directives" do
    source =
      "#when stuff_happens\n" +
      "#ifdef _TEST\n" +
      "#pragma stack_switch"

    expected = []

    assert_equal(expected, @parser.import_source(source))
  end


  it "remove assembler pragma sections" do
    source =
      " #pragma\tasm\n" +
      "  .foo\n" +
      "  lda %m\n" +
      "  nop\n" +
      "# pragma  endasm \n" +
      "foo"

    expected = ["foo"]

    assert_equal(expected, @parser.import_source(source))
  end


  it "smush lines together that contain continuation characters" do
    source =
      "hoo hah \\\n" +
      "when \\ \n"

    expected =
    [
      "hoo hah when"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove C macro definitions" do
    source =
      "#define this is the first line\\\n" +
      "and the second\\\n" +
      "and the third that should be removed\n" +
      "but I'm here\n"

    expected = ["but I'm here"]

    assert_equal(expected, @parser.import_source(source))
  end


  it "remove typedef statements" do
    source =
      "typedef uint32 (unsigned int);\n" +
      "const typedef int INT;\n" +
      "int notatypedef;\n" +
      "int typedef_isnt_me;\n" +
      " typedef who cares what really comes here \n" + # exercise multiline typedef
      "   continuation;\n" +
      "this should remain!;\n" +
      "typedef blah bleh;\n" +
      "typedef struct shell_command_struct {\n" +
      "  char_ptr COMMAND;\n" +
      "  int_32 (*SHELL_FUNC)(int_32 argc);\n" +
      "} SHELL_COMMAND_STRUCT, * SHELL_COMMAND_PTR;\n" +
      "typedef struct shell_command_struct  {\n" +
      "  char_ptr  COMMAND;\n" +
      "  int_32      (*SHELL_FUNC)(int_32 argc, char_ptr argv[]);\n" +
      "} SHELL_COMMAND_STRUCT, * SHELL_COMMAND_PTR;\n" +
      "typedef struct shell_command_struct {\n" +
      " char_ptr COMMAND;\n" +
      " int_32 (*SHELL_FUNC)(int_32 argc);\n" +
      "};\n"

    expected =
    [
      "int notatypedef",
      "int typedef_isnt_me",
      "this should remain!"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove enum statements" do
    source =
      "enum _NamedEnum {\n" +
      " THING1 = (0x0001),\n" +
      " THING2 = (0x0001 << 5),\n" +
      "}ListOValues;\n\n" +
      "don't delete me!!\n" +
      " modifier_str enum _NamedEnum {THING1 = (0x0001), THING2 = (0x0001 << 5)} ListOValues;\n\n" +
      "typedef enum {\n" +
      " THING1,\n" +
      " THING2,\n" +
      "} Thinger;\n" +
      "or me!!\n"

    assert_equal(["don't delete me!! or me!!"], @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove union statements" do
    source =
      "union _NamedDoohicky {\n" +
      " unsigned int a;\n" +
      " char b;\n" +
      "} Doohicky;\n\n" +
      "I want to live!!\n" +
      "some_modifier union { unsigned int a; char b;} Whatever;\n" +
      "typedef union {\n" +
      " unsigned int a;\n" +
      " char b;\n" +
      "} Whatever;\n" +
      "me too!!\n"

    assert_equal(["I want to live!! me too!!"], @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove struct statements" do
    source =
      "struct _NamedStruct1 {\n" +
      " unsigned int a;\n" +
      " signed long int b;\n" +
      "} Thing ;\n\n" +
      "extern struct ForwardDeclared_t TestDataType1;\n" +
      "void foo(void);\n" +
      "struct\n"+
      "   MultilineForwardDeclared_t\n" +
      "   TestDataType2;\n" +
      "struct THINGER foo(void);\n" +
      "typedef struct {\n" +
      " unsigned int a;\n" +
      " signed char b;\n" +
      "}Thinger;\n" +
      "I want to live!!\n"

    assert_equal(["void foo(void)", "struct THINGER foo(void)", "I want to live!!"],
                 @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove externed and inline functions" do
    source =
      " extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "extern void bar(unsigned int);\n" +
      "inline void bar(unsigned int);\n" +
      "extern\n" +
      "void kinda_ugly_on_the_next_line(unsigned int);\n"

    expected =
    [
      "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove function definitions but keep function declarations" do
    source =
      "uint32 func_with_decl_a(unsigned int);\n" +
      "uint32 func_with_decl_a(unsigned int a) { return a; }\n" +
      "uint32 func_with_decl_b(unsigned int);\n" +
      "uint32 func_with_decl_b(unsigned int a)\n" +
      "{\n" +
      "    bar((unsigned int) a);\n" +
      "    stripme(a);\n" +
      "}\n"

    expected =
    [
      "uint32 func_with_decl_a(unsigned int)",
      "uint32 func_with_decl_a",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_b(unsigned int)",
      "uint32 func_with_decl_b",                 #okay. it's not going to be interpretted as another function
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove function definitions with nested braces but keep function declarations" do
    source =
      "uint32 func_with_decl_a(unsigned int);\n" +
      "uint32 func_with_decl_a(unsigned int a) {\n" +
      "  while (stuff) {\n" +
      "    not_a_definition1(void);\n" +
      "  }\n" +
      "  not_a_definition2(blah, bleh);\n" +
      "  return a;\n" +
      "}\n" +
      "uint32 func_with_decl_b(unsigned int);\n" +
      "uint32 func_with_decl_b(unsigned int a)\n" +
      "{\n" +
      "    bar((unsigned int) a);\n" +
      "    stripme(a);\n" +
      "}\n" +
      "uint32 func_with_decl_c(unsigned int);\n" +
      "uint32 func_with_decl_c(unsigned int a)\n" +
      "{\n" +
      "    if(a > 0)\n" +
      "    {\n" +
      "       return 1;\n" +
      "    }\n" +
      "    else\n"+
      "    {\n" +
      "       return 2;\n" +
      "    }\n" +
      "}\n"

    expected =
    [
      "uint32 func_with_decl_a(unsigned int)",
      "uint32 func_with_decl_a",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_b(unsigned int)",
      "uint32 func_with_decl_b",                 #okay. it's not going to be interpretted as another function
      "uint32 func_with_decl_c(unsigned int)",
      "uint32 func_with_decl_c",                 #okay. it's not going to be interpretted as another function
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove a fully defined inline function" do
    source =
      "inline void foo(unsigned int a) { oranges = a; }\n" +
      "inline void bar(unsigned int a) { apples = a; };\n" +
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  bananas = a;\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove a fully defined inline function that is multiple lines" do
    source =
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  bananas = a;\n" +
      "  grapes = a;\n" +
      "  apples(bananas, grapes);\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove a fully defined inline function that contains nested braces" do
    source =
      "inline void bar(unsigned int a)\n" +
      "{" +
      "  apples(bananas, grapes);\n" +
      "  if (bananas == a)\n" +
      "  {\n" +
      "    oranges(a);\n" +
      "    grapes = a;\n" +
      "  }\n" +
      "  grapefruit(bananas, grapes);\n" +
      "}"

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "remove just inline functions if externs to be included" do
    source =
      " extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "extern void bar(unsigned int);\n" +
      "inline void bar(unsigned int);\n" +
      "extern\n" +
      "void kinda_ugly_on_the_next_line(unsigned int);\n"

    expected =
    [ "extern uint32 foobar(unsigned int)",
      "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)",
      "extern void bar(unsigned int)",
      "extern void kinda_ugly_on_the_next_line(unsigned int)"
    ]

    @parser.treat_externs = :include
    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "leave inline functions if inline to be included" do
    source =
      "extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "inline void inlineBar(unsigned int);\n" +
      "extern int extern_bar(void);\n" +
      "static inline void staticinlineBar(unsigned int);\n" +
      "static inline void bar(unsigned int);\n" +
      "static inline void bar(unsigned int)\n" +
      "{\n" +
      " // NOP\n" +
      "}\n"

    expected =
    [ "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)",
      "void inlineBar(unsigned int)",
      "void staticinlineBar(unsigned int)",
      "void bar(unsigned int)"
    ]

    @parser.treat_inlines = :include
    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "leave inline and extern functions if inline and extern to be included" do
    source =
      "extern uint32 foobar(unsigned int);\n" +
      "uint32 extern_name_func(unsigned int);\n" +
      "uint32 funcinline(unsigned int);\n" +
      "inline void inlineBar(unsigned int);\n" +
      "extern int extern_bar(void);\n" +
      "static inline void staticinlineBar(unsigned int);\n" +
      "static inline void bar(unsigned int);\n" +
      "static inline void bar(unsigned int)\n" +
      "{\n" +
      " // NOP\n" +
      "}\n"

    expected =
    [ "extern uint32 foobar(unsigned int)",
      "uint32 extern_name_func(unsigned int)",
      "uint32 funcinline(unsigned int)",
      "void inlineBar(unsigned int)",
      "extern int extern_bar(void)",
      "void staticinlineBar(unsigned int)",
      "void bar(unsigned int)"
    ]

    @parser.treat_externs = :include
    @parser.treat_inlines = :include
    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "Include inline functions that contain user defined inline function formats" do
    source =
      "uint32 foo(unsigned int);\n" +
      "uint32 bar(unsigned int);\n" +
      "inline void inlineBar(void)\n" +
      "{\n" +
      "    return 43;\n" +
      "}\n" +
      "static __inline__ __attribute__ ((always_inline)) int alwaysinlinefunc(int a)\n" +
      "{\n" +
      "    return a + inlineBar();\n" +
      "}\n" +
      "static __inline__ void inlinebar(unsigned int)\n" +
      "{\n" +
      " int a = alwaysinlinefunc()\n" +
      "}\n"

    expected =
      [
      "uint32 foo(unsigned int)",
      "uint32 bar(unsigned int)",
      "void inlineBar(void)",
      "int alwaysinlinefunc(int a)",
      "void inlinebar(unsigned int)"
      ]

    @parser.treat_inlines = :include
    @parser.inline_function_patterns = ['static __inline__ __attribute__ \(\(always_inline\)\)', 'static __inline__', '\binline\b']
    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end

  it "remove defines" do
    source =
      "#define whatever you feel like defining\n" +
      "void hello(void);\n" +
      "#DEFINE I JUST DON'T CARE\n" +
      "#deFINE\n" +
      "#define get_foo() \\\n   ((Thing)foo.bar)" # exercise multiline define

    expected =
    [
      "void hello(void)",
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "remove keywords that would keep things from going smoothly in the future" do
    source =
      "const int TheMatrix(register int Trinity, unsigned int *restrict Neo)"

    expected =
    [
      "const int TheMatrix(int Trinity, unsigned int * Neo)",
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  # some code actually typedef's void even though it's not ANSI C and is, frankly, weird
  # since cmock treats void specially, we can't let void be obfuscated
  it "handle odd case of typedef'd void returned" do
    source = "MY_FUNKY_VOID FunkyVoidReturned(int a)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidReturned",
                 :unscoped_name=>"FunkyVoidReturned",
                 :namespace=>[],
                 :class=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[{:type=>"int", :name=>"a", :ptr? => false, :const? => false, :const_ptr? => false}],
                 :args_string=>"int a",
                 :args_call=>"a"}
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "handle odd case of typedef'd void as arg" do
    source = "int FunkyVoidAsArg(MY_FUNKY_VOID)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidAsArg",
                 :unscoped_name=>"FunkyVoidAsArg",
                 :namespace=>[],
                 :class=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "handle odd case of typedef'd void as arg pointer" do
    source = "char FunkyVoidPointer(MY_FUNKY_VOID* bluh)"
    expected = { :var_arg=>nil,
                 :name=>"FunkyVoidPointer",
                 :unscoped_name=>"FunkyVoidPointer",
                 :namespace=>[],
                 :class=>nil,
                 :return=>{ :type   => "char",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "char cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[{:type=>"MY_FUNKY_VOID*", :name=>"bluh", :ptr? => true, :const? => false, :const_ptr? => false}],
                 :args_string=>"MY_FUNKY_VOID* bluh",
                 :args_call=>"bluh" }
    assert_equal(expected, @parser.parse_declaration(source))
  end


  it "strip default values from function parameter lists" do
    source =
      "void Foo(int a = 57, float b=37.52, char c= 'd', char* e=\"junk\");\n"

    expected =
    [
      "void Foo(int a, float b, char c, char* e)"
    ]

    assert_equal(expected, @parser.import_source(source).map!{|s|s.strip})
  end


  it "raise upon empty file" do
    source = ''

    # ensure it's expected type of exception
    assert_raises RuntimeError do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end

  it "clean up module names that contain spaces, dashes, and such" do
    source = 'void meh(int (*func)(int));'

    retval = @parser.parse("C:\Ugly Module-Name", source)
    assert (retval[:typedefs][0] =~ /CUglyModuleName/)
  end

  it "raise upon no function prototypes found in file" do
    source =
      "typedef void SILLY_VOID_TYPE1;\n" +
      "typedef (void) SILLY_VOID_TYPE2 ;\n" +
      "typedef ( void ) (*FUNCPTR)(void);\n\n" +
      "#define get_foo() \\\n   ((Thing)foo.bar)"

    # ensure it's expected type of exception
    assert_raises(RuntimeError) do
      @parser.parse("module", source)
    end

    assert_equal([], @parser.funcs)

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert_equal("ERROR: No function prototypes found!", e.message)
    end
  end


  it "raise upon prototype parsing failure" do
    source = "void (int, )"

    # ensure it's expected type of exception
    assert_raises(RuntimeError) do
      @parser.parse("module", source)
    end

    # verify exception message
    begin
      @parser.parse("module", source)
    rescue RuntimeError => e
      assert(e.message.include?("Failed Parsing Declaration Prototype!"))
    end
  end

  it "extract and return function declarations with retval and args" do

    source = "int Foo(int a, unsigned int b)"
    expected = { :var_arg=>nil,
                 :name=>"Foo",
                 :unscoped_name=>"Foo",
                 :namespace=>[],
                 :class=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"int", :name=>"a", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int", :name=>"b", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int a, unsigned int b",
                 :args_call=>"a, b" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract and return function declarations with no retval" do

    source = "void    FunkyChicken(    uint la,  int     de, bool da)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyChicken",
                 :unscoped_name=>"FunkyChicken",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"uint", :name=>"la", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int",  :name=>"de", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"bool", :name=>"da", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"uint la, int     de, bool da",
                 :args_call=>"la, de, da" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract and return function declarations with implied voids" do

    source = "void tat()"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"tat",
                 :unscoped_name=>"tat",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ ],
                 :args_string=>"void",
                 :args_call=>"" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract modifiers properly" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => true,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :name=>"TheMatrix",
                 :unscoped_name=>"TheMatrix",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"const",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract c calling conventions properly" do

    source = "const int __stdcall TheMatrix(int Trinity, unsigned int * Neo)"
    expected = { :var_arg=>nil,
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => true,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :name=>"TheMatrix",
                 :unscoped_name=>"TheMatrix",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"const",
                 :c_calling_convention=>"__stdcall",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" }
    assert_equal(expected, @parser.parse_declaration(source))
  end

  it "extract and return function declarations inside namespace and class" do
    source = "int Foo(int a, unsigned int b)"
    expected = { :var_arg=>nil,
                 :name=>"ns1_ns2_Bar_Foo",
                 :unscoped_name=>"Foo",
                 :class=>"Bar",
                 :namespace=>["ns1", "ns2"],
                 :return=>{ :type   => "int",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "int cmock_to_return",
                            :void?  => false
                          },
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"int", :name=>"a", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned int", :name=>"b", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"int a, unsigned int b",
                 :args_call=>"a, b" }
    assert_equal(expected, @parser.parse_declaration(source, ["ns1", "ns2"], "Bar"))
  end

  it "fully parse multiple prototypes" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo);\n" +
             "int Morpheus(int, unsigned int*);\n"

    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => true,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"TheMatrix",
                  :unscoped_name=>"TheMatrix",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"const",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"Neo",     :ptr? => true,  :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Trinity, unsigned int* Neo",
                 :args_call=>"Trinity, Neo" },
                { :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"Morpheus",
                  :unscoped_name=>"Morpheus",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"cmock_arg1", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"cmock_arg2", :ptr? => true,  :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int cmock_arg1, unsigned int* cmock_arg2",
                 :args_call=>"cmock_arg1, cmock_arg2"
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "not extract for mocking multiply defined prototypes" do

    source = "const int TheMatrix(int Trinity, unsigned int * Neo);\n" +
             "const int TheMatrix(int, unsigned int*);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"TheMatrix",
                  :unscoped_name=>"TheMatrix",
                  :namespace=>[],
                  :class=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => true,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :modifier=>"const",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"int",           :name=>"Trinity", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"unsigned int*", :name=>"Neo", :ptr? => true,      :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Trinity, unsigned int* Neo",
                  :args_call=>"Trinity, Neo"
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "should properly handle const before return type" do
    sources = [
      "const int * PorkRoast(void);\n",
      "const int* PorkRoast(void);\n",
      "const int *PorkRoast(void);\n"
    ]

    expected = [{ :var_arg => nil,
                  :name    => "PorkRoast",
                  :unscoped_name => "PorkRoast",
                  :namespace=>[],
                  :class=>nil,
                  :return  => { :type       => "const int*",
                                :name       => 'cmock_to_return',
                                :ptr?       => true,
                                :const?     => true,
                                :const_ptr? => false,
                                :str        => "const int* cmock_to_return",
                                :void?      => false
                              },
                  :modifier      => "",
                  :contains_ptr? => false,
                  :args          => [],
                  :args_string   => "void",
                  :args_call     => ""
                }]

    sources.each do |source|
      assert_equal(expected, @parser.parse("module", source)[:functions])
    end
  end

  it "should properly handle const before return type" do
    sources = [
      "int const * PorkRoast(void);\n",
      "int const* PorkRoast(void);\n",
      "int const *PorkRoast(void);\n"
    ]

    expected = [{ :var_arg => nil,
                  :name    => "PorkRoast",
                  :unscoped_name => "PorkRoast",
                  :namespace=>[],
                  :class=>nil,
                  :return  => { :type       => "int const*",
                                :name       => 'cmock_to_return',
                                :ptr?       => true,
                                :const?     => true,
                                :const_ptr? => false,
                                :str        => "int const* cmock_to_return",
                                :void?      => false
                              },
                  :modifier      => "",
                  :contains_ptr? => false,
                  :args          => [],
                  :args_string   => "void",
                  :args_call     => ""
                }]

    sources.each do |source|
      assert_equal(expected, @parser.parse("module", source)[:functions])
    end
  end

  it "should properly handle const applied after asterisk in return type (not legal C, but sometimes used)" do

    source = "int * const PorkRoast(void);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"PorkRoast",
                  :unscoped_name=>"PorkRoast",
                  :namespace=>[],
                  :class=>nil,
                  :return=> { :type   => "int*",
                              :name   => 'cmock_to_return',
                              :ptr?   => true,
                              :const? => false,
                              :const_ptr? => true,
                              :str    => "int* cmock_to_return",
                              :void?  => false
                            },
                  :modifier=>"const",
                  :contains_ptr? => false,
                  :args=>[],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "properly parse const and pointer argument types with no arg names" do

    source = "void foo(int const*, int*const, const int*, const int*const, int const*const, int*, int, const int);\n"

    expected = [{ :name => "foo",
                  :unscoped_name => "foo",
                  :namespace=>[],
                  :class=>nil,
                  :modifier => "",
                  :return => { :type       => "void",
                               :name       => "cmock_to_return",
                               :str        => "void cmock_to_return",
                               :void?      => true,
                               :ptr?       => false,
                               :const?     => false,
                               :const_ptr? => false
                             },
                  :var_arg => nil,
                  :args_string => "int const* cmock_arg1, int* const cmock_arg2, const int* cmock_arg3, const int* const cmock_arg4, " +
                                  "int const* const cmock_arg5, int* cmock_arg6, int cmock_arg7, const int cmock_arg8",
                  :args => [{ :type=>"int const*", :name => "cmock_arg1", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"int*",       :name => "cmock_arg2", :ptr? => true,  :const? => false, :const_ptr? => true  },
                            { :type=>"const int*", :name => "cmock_arg3", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"const int*", :name => "cmock_arg4", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int const*", :name => "cmock_arg5", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int*",       :name => "cmock_arg6", :ptr? => true,  :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "cmock_arg7", :ptr? => false, :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "cmock_arg8", :ptr? => false, :const? => true,  :const_ptr? => false }],
                  :args_call => "cmock_arg1, cmock_arg2, cmock_arg3, cmock_arg4, cmock_arg5, cmock_arg6, cmock_arg7, cmock_arg8",
                  :contains_ptr? => true
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "properly parse const and pointer argument types with arg names" do

    source = "void bar(int const* param1, int*const param2, const int* param3, const int*const param4,\n" +
             "         int const*const param5, int*param6, int param7, const int param8);\n"

    expected = [{ :name => "bar",
                  :unscoped_name => "bar",
                  :namespace=>[],
                  :class=>nil,
                  :modifier => "",
                  :return => { :type       => "void",
                               :name       => "cmock_to_return",
                               :str        => "void cmock_to_return",
                               :void?      => true,
                               :ptr?       => false,
                               :const?     => false,
                               :const_ptr? => false
                             },
                  :var_arg => nil,
                  :args_string => "int const* param1, int* const param2, const int* param3, const int* const param4, " +
                                  "int const* const param5, int* param6, int param7, const int param8",
                  :args => [{ :type=>"int const*", :name => "param1", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"int*",       :name => "param2", :ptr? => true,  :const? => false, :const_ptr? => true  },
                            { :type=>"const int*", :name => "param3", :ptr? => true,  :const? => true,  :const_ptr? => false },
                            { :type=>"const int*", :name => "param4", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int const*", :name => "param5", :ptr? => true,  :const? => true,  :const_ptr? => true  },
                            { :type=>"int*",       :name => "param6", :ptr? => true,  :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "param7", :ptr? => false, :const? => false, :const_ptr? => false },
                            { :type=>"int",        :name => "param8", :ptr? => false, :const? => true,  :const_ptr? => false }],
                  :args_call => "param1, param2, param3, param4, param5, param6, param7, param8",
                  :contains_ptr? => true
                }].freeze
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "converts typedef'd array arguments to pointers" do

    source = "Book AddToBook(Book book, const IntArray values);\n"

    expected = [{ :name => "AddToBook",
                  :unscoped_name => "AddToBook",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :return  => { :type       => "Book",
                                :name       => "cmock_to_return",
                                :str        => "Book cmock_to_return",
                                :void?      => false,
                                :ptr?       => false,
                                :const?     => false,
                                :const_ptr? => false
                              },
                  :var_arg => nil,
                  :args => [{ :type => "Page*",      :name => "book",   :ptr? => true, :const? => false, :const_ptr? => false },
                            { :type => "const int*", :name => "values", :ptr? => true, :const? => true,  :const_ptr? => false }],
                  :args_string => "Book book, const IntArray values",
                  :args_call => "book, values",
                  :contains_ptr? => true
                }]

    assert_equal(expected, @parser.parse("module", source)[:functions])

  end

  it "properly detect typedef'd variants of void and use those" do

    source = "typedef (void) FUNKY_VOID_T;\n" +
             "typedef void CHUNKY_VOID_T;\n" +
             "FUNKY_VOID_T DrHorrible(int SingAlong);\n" +
             "int CaptainHammer(CHUNKY_VOID_T);\n"

    expected = [{ :var_arg=>nil,
                  :name=>"DrHorrible",
                  :unscoped_name=>"DrHorrible",
                  :namespace=>[],
                  :class=>nil,
                  :return  => { :type   => "void",
                                :name   => 'cmock_to_return',
                                :ptr?   => false,
                                :const? => false,
                                :const_ptr? => false,
                                :str    => "void cmock_to_return",
                                :void?  => true
                              },
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"SingAlong", :ptr? => false, :const? => false, :const_ptr? => false} ],
                  :args_string=>"int SingAlong",
                  :args_call=>"SingAlong"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"CaptainHammer",
                  :unscoped_name=>"CaptainHammer",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ ],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "be ok with structs inside of function declarations" do

    source = "int DrHorrible(struct SingAlong Blog);\n" +
             "void Penny(struct const _KeepYourHeadUp_ * const BillyBuddy);\n" +
             "struct TheseArentTheHammer CaptainHammer(void);\n"

    expected = [{ :var_arg=>nil,
                  :return =>{ :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"DrHorrible",
                  :unscoped_name=>"DrHorrible",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"struct SingAlong", :name=>"Blog", :ptr? => false, :const? => false, :const_ptr? => false} ],
                  :args_string=>"struct SingAlong Blog",
                  :args_call=>"Blog"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "void",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "void cmock_to_return",
                              :void?  => true
                            },
                  :name=>"Penny",
                  :unscoped_name=>"Penny",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"struct const _KeepYourHeadUp_*", :name=>"BillyBuddy", :ptr? => true, :const? => true, :const_ptr? => true} ],
                  :args_string=>"struct const _KeepYourHeadUp_* const BillyBuddy",
                  :args_call=>"BillyBuddy"
                },
                { :var_arg=>nil,
                  :return=> { :type   => "struct TheseArentTheHammer",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "struct TheseArentTheHammer cmock_to_return",
                              :void?  => false
                            },
                  :name=>"CaptainHammer",
                  :unscoped_name=>"CaptainHammer",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ ],
                  :args_string=>"void",
                  :args_call=>""
                }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions containing unions with union specifier" do
    source = "void OrangePeel(union STARS_AND_STRIPES * a, union AFL_CIO b)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"OrangePeel",
                 :unscoped_name=>"OrangePeel",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"union STARS_AND_STRIPES*", :name=>"a", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"union AFL_CIO", :name=>"b", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"union STARS_AND_STRIPES* a, union AFL_CIO b",
                 :args_call=>"a, b" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "not be thwarted by variables named with primitive types as part of the name" do
    source = "void ApplePeel(const unsigned int const_param, int int_param, int integer, char character, int* const constant)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"ApplePeel",
                 :unscoped_name=>"ApplePeel",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=> "unsigned int", :name=>"const_param", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"int", :name=>"int_param", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int", :name=>"integer", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"char", :name=>"character", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int*", :name=>"constant", :ptr? => true, :const? => false, :const_ptr? => true}
                        ],
                 :args_string=>"const unsigned int const_param, int int_param, int integer, char character, int* const constant",
                 :args_call=>"const_param, int_param, integer, character, constant" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "not be thwarted by custom types named similarly to primitive types" do
    source = "void LemonPeel(integer param, character thing, longint * junk, constant value, int32_t const number)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"LemonPeel",
                 :unscoped_name=>"LemonPeel",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"integer", :name=>"param", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"character", :name=>"thing", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"longint*", :name=>"junk", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"constant", :name=>"value", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"int32_t", :name=>"number", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"integer param, character thing, longint* junk, constant value, int32_t const number",
                 :args_call=>"param, thing, junk, value, number" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle some of those chains of C name specifiers naturally" do
    source = "void CoinOperated(signed char abc, const unsigned long int xyz_123, unsigned int const abc_123, long long arm_of_the_law)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"CoinOperated",
                 :unscoped_name=>"CoinOperated",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"signed char", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned long int", :name=>"xyz_123", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"unsigned int", :name=>"abc_123", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"long long", :name=>"arm_of_the_law", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"signed char abc, const unsigned long int xyz_123, unsigned int const abc_123, long long arm_of_the_law",
                 :args_call=>"abc, xyz_123, abc_123, arm_of_the_law" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle custom types of various formats" do
    source = "void CardOperated(CUSTOM_TYPE abc, CUSTOM_TYPE* xyz_123, CUSTOM_TYPE const abcxyz, struct CUSTOM_TYPE const * const abc123)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"CardOperated",
                 :unscoped_name=>"CardOperated",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"CUSTOM_TYPE", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE*", :name=>"xyz_123", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE", :name=>"abcxyz", :ptr? => false, :const? => true, :const_ptr? => false},
                          {:type=>"struct CUSTOM_TYPE const*", :name=>"abc123", :ptr? => true, :const? => true, :const_ptr? => true}
                        ],
                 :args_string=>"CUSTOM_TYPE abc, CUSTOM_TYPE* xyz_123, CUSTOM_TYPE const abcxyz, struct CUSTOM_TYPE const* const abc123",
                 :args_call=>"abc, xyz_123, abcxyz, abc123" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "handle arrays and treat them as pointers or strings" do
    source = 'void KeyOperated(CUSTOM_TYPE thing1[], int thing2 [ ], ' \
             'char thing3 [][2 ][ 3], int* thing4[4], u8 thing5[((u8)((5 + 5*2)/3))])'
    expected_args = [
      { type: 'CUSTOM_TYPE*', name: 'thing1', ptr?: true,  const?: false, const_ptr?: false },
      { type: 'int*',         name: 'thing2', ptr?: true,  const?: false, const_ptr?: false },
      # this one will likely change in the future when we improve multidimensional array support
      { type: 'char*',        name: 'thing3', ptr?: false, const?: false, const_ptr?: false },
      # this one will likely change in the future when we improve multidimensional array support
      { type: 'int**',        name: 'thing4', ptr?: true,  const?: false, const_ptr?: false },
      { type: 'u8*',          name: 'thing5', ptr?: true,  const?: false, const_ptr?: false }
    ]
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"KeyOperated",
                 :unscoped_name=>"KeyOperated",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args => expected_args,
                 :args_string => 'CUSTOM_TYPE* thing1, int* thing2, ' \
                                 'char* thing3, int** thing4, u8* thing5',
                 :args_call => 'thing1, thing2, thing3, thing4, thing5' }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "give a reasonable guess when dealing with weird combinations of custom types and modifiers" do
    source = "void Cheese(unsigned CUSTOM_TYPE abc, unsigned xyz, CUSTOM_TYPE1 CUSTOM_TYPE2 pdq)"
    expected = [{:var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"Cheese",
                 :unscoped_name=>"Cheese",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"unsigned CUSTOM_TYPE", :name=>"abc", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"unsigned", :name=>"xyz", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"CUSTOM_TYPE1 CUSTOM_TYPE2", :name=>"pdq", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"unsigned CUSTOM_TYPE abc, unsigned xyz, CUSTOM_TYPE1 CUSTOM_TYPE2 pdq",
                 :args_call=>"abc, xyz, pdq" }]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
  end

  it "extract functions containing a function pointer" do
    source = "void FunkyTurkey(unsigned int (*func_ptr)(int, char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :unscoped_name=>"FunkyTurkey",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions using a function pointer with shorthand notation" do
    source = "void FunkyTurkey(unsigned int func_ptr(int, char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :unscoped_name=>"FunkyTurkey",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointer with a void" do
    source = "void FunkyTurkey(void (*func_ptr)(void))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :unscoped_name=>"FunkyTurkey",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointer with an implied void" do
    source = "void FunkyTurkey(unsigned int (*func_ptr)())"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyTurkey",
                 :unscoped_name=>"FunkyTurkey",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)();"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a constant function pointer and a pointer in the nested arg list" do
    source = "void FunkyChicken(unsigned int (* const func_ptr)(unsigned long int * , char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyChicken",
                 :unscoped_name=>"FunkyChicken",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 const func_ptr",
                 :args_call=>"func_ptr" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(unsigned long int* , char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  # it "extract functions containing a function pointer taking a vararg" do
    # source = "void FunkyParrot(unsigned int (*func_ptr)(int, char, ...))"
    # expected = [{ :var_arg=>nil,
                 # :return=>{ :type   => "void",
                            # :name   => 'cmock_to_return',
                            # :ptr?   => false,
                            # :const? => false,
                            # :const_ptr? => false,
                            # :str    => "void cmock_to_return",
                            # :void?  => true
                          # },
                 # :name=>"FunkyParrot",
                 # :unscoped_name=>"FunkyParrot",
                 # :namespace=>[],
                 # :class=>nil,
                 # :modifier=>"",
                 # :contains_ptr? => false,
                 # :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr", :ptr? => false, :const? => false, :const_ptr? => false}
                        # ],
                 # :args_string=>"cmock_module_func_ptr1 func_ptr",
                 # :args_call=>"func_ptr" }]
    # typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char, ...);"]
    # result = @parser.parse("module", source)
    # assert_equal(expected, result[:functions])
    # assert_equal(typedefs, result[:typedefs])
  # end

  it "extract functions containing a function pointer with extra parenthesis and two sets" do
    source = "void FunkyBudgie(int (((* func_ptr1)(int, char))), void (*func_ptr2)(void))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyBudgie",
                 :unscoped_name=>"FunkyBudgie",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"func_ptr1", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"cmock_module_func_ptr2", :name=>"func_ptr2", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 func_ptr1, cmock_module_func_ptr2 func_ptr2",
                 :args_call=>"func_ptr1, func_ptr2" }]
    typedefs = ["typedef int(*cmock_module_func_ptr1)(int, char);", "typedef void(*cmock_module_func_ptr2)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing a function pointers, structs and other things" do
    source = "struct mytype *FunkyRobin(uint16_t num1, uint16_t num2, void (*func_ptr1)(uint16_t num3, struct mytype2 *s));"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "struct mytype*",
                            :name   => 'cmock_to_return',
                            :ptr?   => true,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "struct mytype* cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyRobin",
                 :unscoped_name=>"FunkyRobin",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"uint16_t", :name=>"num1", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"uint16_t", :name=>"num2", :ptr? => false, :const? => false, :const_ptr? => false},
                          {:type=>"cmock_module_func_ptr1", :name=>"func_ptr1", :ptr? => false, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"uint16_t num1, uint16_t num2, cmock_module_func_ptr1 func_ptr1",
                 :args_call=>"num1, num2, func_ptr1" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(uint16_t num3, struct mytype2* s);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions containing an anonymous function pointer" do
    source = "void FunkyFowl(unsigned int (* const)(int, char))"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "void",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "void cmock_to_return",
                            :void?  => true
                          },
                 :name=>"FunkyFowl",
                 :unscoped_name=>"FunkyFowl",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"cmock_module_func_ptr1", :name=>"cmock_arg1", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"cmock_module_func_ptr1 const cmock_arg1",
                 :args_call=>"cmock_arg1" }]
    typedefs = ["typedef unsigned int(*cmock_module_func_ptr1)(int, char);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer" do
    source = "unsigned short (*FunkyPidgeon( const char op_code ))( int, long int )"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyPidgeon",
                 :unscoped_name=>"FunkyPidgeon",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[ {:type=>"char", :name=>"op_code", :ptr? => false, :const? => true, :const_ptr? => false}
                        ],
                 :args_string=>"const char op_code",
                 :args_call=>"op_code" }]
    typedefs = ["typedef unsigned short(*cmock_module_func_ptr1)( int, long int );"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer with implied void" do
    source = "unsigned short (*FunkyTweetie())()"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyTweetie",
                 :unscoped_name=>"FunkyTweetie",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }]
    typedefs = ["typedef unsigned short(*cmock_module_func_ptr1)();"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer where everything is a void" do
    source = "void (*   FunkySeaGull(void))(void)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkySeaGull",
                 :unscoped_name=>"FunkySeaGull",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => false,
                 :args=>[],
                 :args_string=>"void",
                 :args_call=>"" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions returning a function pointer with some pointer nonsense" do
    source = "unsigned int * (* FunkyMacaw(double* foo, THING *bar))(unsigned int)"
    expected = [{ :var_arg=>nil,
                 :return=>{ :type   => "cmock_module_func_ptr1",
                            :name   => 'cmock_to_return',
                            :ptr?   => false,
                            :const? => false,
                            :const_ptr? => false,
                            :str    => "cmock_module_func_ptr1 cmock_to_return",
                            :void?  => false
                          },
                 :name=>"FunkyMacaw",
                 :unscoped_name=>"FunkyMacaw",
                 :namespace=>[],
                 :class=>nil,
                 :modifier=>"",
                 :contains_ptr? => true,
                 :args=>[ {:type=>"double*", :name=>"foo", :ptr? => true, :const? => false, :const_ptr? => false},
                          {:type=>"THING*", :name=>"bar", :ptr? => true, :const? => false, :const_ptr? => false}
                        ],
                 :args_string=>"double* foo, THING* bar",
                 :args_call=>"foo, bar" }]
    typedefs = ["typedef unsigned int *(*cmock_module_func_ptr1)(unsigned int);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract this SQLite3 function with an anonymous function pointer arg (regression test)" do
    source = "SQLITE_API int sqlite3_bind_text(sqlite3_stmt*, int, const char*, int n, void(*)(void*))"
    expected = [{ :var_arg=>nil,
                  :return=>{ :type   => "int",
                             :name   => "cmock_to_return",
                             :ptr?   => false,
                             :const? => false,
                             :const_ptr? => false,
                             :str    => "int cmock_to_return",
                             :void?  => false
                           },
                   :name=>"sqlite3_bind_text",
                   :unscoped_name=>"sqlite3_bind_text",
                   :namespace=>[],
                   :class=>nil,
                   :modifier=>"SQLITE_API",
                   :contains_ptr? => true,
                   :args=>[ {:type=>"sqlite3_stmt*", :name=>"cmock_arg2", :ptr? => true, :const? => false, :const_ptr? => false},
                            {:type=>"int", :name=>"cmock_arg3", :ptr? => false, :const? => false, :const_ptr? => false},
                            {:type=>"const char*", :name=>"cmock_arg4", :ptr? => false, :const? => true, :const_ptr? => false},
                            {:type=>"int", :name=>"n", :ptr? => false, :const? => false, :const_ptr? => false},
                            {:type=>"cmock_module_func_ptr1", :name=>"cmock_arg1", :ptr? => false, :const? => false, :const_ptr? => false}
                          ],
                   :args_string=>"sqlite3_stmt* cmock_arg2, int cmock_arg3, const char* cmock_arg4, int n, cmock_module_func_ptr1 cmock_arg1",
                   :args_call=>"cmock_arg2, cmock_arg3, cmock_arg4, n, cmock_arg1" }]
    typedefs = ["typedef void(*cmock_module_func_ptr1)(void*);"]
    result = @parser.parse("module", source)
    assert_equal(expected, result[:functions])
    assert_equal(typedefs, result[:typedefs])
  end

  it "extract functions with varargs" do
    source = "int XFiles(int Scully, int Mulder, ...);\n"
    expected = [{ :var_arg=>"...",
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"XFiles",
                  :unscoped_name=>"XFiles",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Scully", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Mulder", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Scully, int Mulder",
                  :args_call=>"Scully, Mulder"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with void pointers" do
    source = "void* MoreSillySongs(void* stuff);\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "void*",
                              :name   => 'cmock_to_return',
                              :ptr?   => true,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "void* cmock_to_return",
                              :void?  => false
                            },
                  :name=>"MoreSillySongs",
                  :unscoped_name=>"MoreSillySongs",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => true,
                  :args=>[ {:type=>"void*", :name=>"stuff", :ptr? => true, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"void* stuff",
                  :args_call=>"stuff"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with strippable confusing junk like gcc attributes" do
    source = "int LaverneAndShirley(int Lenny, int Squiggy) __attribute__((weak)) __attribute__ ((deprecated));\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"LaverneAndShirley",
                  :unscoped_name=>"LaverneAndShirley",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Lenny", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Squiggy", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Lenny, int Squiggy",
                  :args_call=>"Lenny, Squiggy"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "extract functions with strippable confusing junk like gcc attributes with parenthesis" do
    source = "int TheCosbyShow(int Cliff, int Claire) __attribute__((weak, alias (\"__f\"));\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"TheCosbyShow",
                  :unscoped_name=>"TheCosbyShow",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Cliff", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Claire", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Cliff, int Claire",
                  :args_call=>"Cliff, Claire"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "divines all permutations of ptr, const, and const_ptr correctly" do
    truth_table = [
      # argument                                           ptr    const  const_ptr
      [ "constNOTconst constNOTconst",                     false, false, false ],
      [ "const constNOTconst constNOTconst",               false, true,  false ],
      [ "constNOTconst const constNOTconst",               false, true,  false ],
      [ "constNOTconst *constNOTconst",                    true,  false, false ],
      [ "const constNOTconst *constNOTconst",              true,  true,  false ],
      [ "constNOTconst const *constNOTconst",              true,  true,  false ],
      [ "constNOTconst *const constNOTconst",              true,  false, true ],
      [ "const constNOTconst *const constNOTconst",        true,  true,  true ],
      [ "constNOTconst const *const constNOTconst",        true,  true,  true ],
      [ "constNOTconst **constNOTconst",                   true,  false, false ],
      [ "const constNOTconst **constNOTconst",             true,  false, false ],
      [ "constNOTconst const **constNOTconst",             true,  false, false ],
      [ "constNOTconst *const *constNOTconst",             true,  true,  false ],
      [ "const constNOTconst *const *constNOTconst",       true,  true,  false ],
      [ "constNOTconst const *const *constNOTconst",       true,  true,  false ],
      [ "constNOTconst **const constNOTconst",             true,  false, true ],
      [ "const constNOTconst **const constNOTconst",       true,  false, true ],
      [ "constNOTconst const **const constNOTconst",       true,  false, true ],
      [ "constNOTconst *const *const constNOTconst",       true,  true,  true ],
      [ "const constNOTconst *const *const constNOTconst", true,  true,  true ],
      [ "constNOTconst const *const *const constNOTconst", true,  true,  true ]
    ]

    truth_table.each do |entry|
      assert_equal(@parser.divine_ptr(entry[0]), entry[1])
      assert_equal(@parser.divine_const(entry[0]), entry[2])
      assert_equal(@parser.divine_ptr_and_const(entry[0]),
        { ptr?: entry[1], const?: entry[2], const_ptr?: entry[3] })
    end
  end

  it "divines ptr correctly for string types" do
    truth_table = [
      # argument                      ptr
      [ "char s",                     false ],
      [ "const char s",               false ],
      [ "char const s",               false ],
      [ "char *s",                    false ],
      [ "const char *s",              false ],
      [ "char const *s",              false ],
      [ "char *const s",              false ],
      [ "const char *const s",        false ],
      [ "char const *const s",        false ],
      [ "char **s",                   true  ],
      [ "const char **s",             true  ],
      [ "char const **s",             true  ],
      [ "char *const *s",             true  ],
      [ "const char *const *s",       true  ],
      [ "char const *const *s",       true  ],
      [ "char **const s",             true  ],
      [ "const char **const s",       true  ],
      [ "char const **const s",       true  ],
      [ "char *const *const s",       true  ],
      [ "const char *const *const s", true  ],
      [ "char const *const *const s", true  ]
    ]

    truth_table.each do |entry|
      assert_equal(@parser.divine_ptr(entry[0]), entry[1])
    end
  end

  it "Transform inline functions doesn't change a header with no inlines" do
    source =
      "#ifndef _NOINCLUDES\n" +
      "#define _NOINCLUDES\n" +
      "#include \"unity.h\"\n" +
      "#include \"cmock.h\"\n" +
      "#include \"YetAnotherHeader.h\"\n" +
      "\n" +
      "/* Ignore the following warnings since we are copying code */\n" +
      "#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)\n" +
      "#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))\n" +
      "#pragma GCC diagnostic push\n" +
      "#endif\n" +
      "#if !defined(__clang__)\n" +
      "#pragma GCC diagnostic ignored \"-Wpragmas\"\n" +
      "#endif\n" +
      "#pragma GCC diagnostic ignored \"-Wunknown-pragmas\"\n" +
      "#pragma GCC diagnostic ignored \"-Wduplicate-decl-specifier\"\n" +
      "#endif\n" +
      "\n" +
      "struct my_struct {\n" +
      "int a;\n" +
      "int b;\n" +
      "int b;\n" +
      "char c;\n" +
      "};\n" +
      "int my_function(int a);\n" +
      "int my_better_function(struct my_struct *s);\n" +
      "\n" +
      "#endif _NOINCLUDES\n"

    assert_equal(source, @parser.transform_inline_functions(source))
  end

  it "Transform inline functions changes inline functions to function declarations" do
    source =
      "#ifndef _NOINCLUDES\n" +
      "#define _NOINCLUDES\n" +
      "#include \"unity.h\"\n" +
      "#include \"cmock.h\"\n" +
      "#include \"YetAnotherHeader.h\"\n" +
      "\n" +
      "/* Ignore the following warnings since we are copying code */\n" +
      "#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)\n" +
      "#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))\n" +
      "#pragma GCC diagnostic push\n" +
      "#endif\n" +
      "#if !defined(__clang__)\n" +
      "#pragma GCC diagnostic ignored \"-Wpragmas\"\n" +
      "#endif\n" +
      "#pragma GCC diagnostic ignored \"-Wunknown-pragmas\"\n" +
      "#pragma GCC diagnostic ignored \"-Wduplicate-decl-specifier\"\n" +
      "#endif\n" +
      "\n" +
      "struct my_struct {\n" +
      "int a;\n" +
      "int b;\n" +
      "int b;\n" +
      "char c;\n" +
      "};\n" +
      "int my_function(int a);\n" +
      "int my_better_function(struct my_struct *s);\n" +
      "static inline int staticinlinebar(struct my_struct *s)\n" + # This is a function with a lot of indentation levels, we should be able to handle it
      "{\n" +
      "\t{\n" +
      "\t\t{\n" +
      "\t\t\treturn s->a;\n" +
      "\t\t}\n" +
      "\t}\n" +
      "}\n" +
      "static inline int staticinlinefunc(struct my_struct *s)\n" +
      "{\n" +
      "    return s->a;\n" +
      "}\n" +
      "int bar(struct my_struct *s);\n" + # A normal function to screw with our parser
      "inline static int inlinestaticfunc(int a) {\n" +
      "    return a + 42;\n" +
      "}\n" +
      "inline int StaticInlineFunc(struct my_struct *s)\n" +
      "{\n" +
      "    return get_member_a(s) + 42;\n" +
      "}\n" +
      "int inline StaticInlineBar(struct my_struct *s)\n" +
      "{\n" +
      "    return get_member_a(s) + 42;\n" +
      "}\n" +
      "struct staticinlinestruct {\n" + # Add a structure declaration between the inline functions, just to make sure we don't touch it!
      "int a;\n" +
      "};\n" +
      "\n" +
      "struct staticinlinestruct fubarstruct(struct my_struct *s);\n" + # Another normal function to screw with our parser
      "static inline struct staticinlinestruct inlinefubarfunction(struct my_struct *s)\n" +
      "{\n" +
      "    return (struct staticinlinestruct)*s;\n" +
      "}\n" +
      "int fubar(struct my_struct *s);\n" + # Another normal function to screw with our parser
      "inline int stuff(int num)" +
      "{" +
      "    int reg = 0x12;" +
      "    if (num > 0)" +
      "    {" +
      "      reg |= (0x0Eu);" +
      "    }" +
      "    else" +
      "    {" +
      "      reg |= (0x07u);" +
      "    }" +
      "    return reg;" +
      "}" +
      "\n" +
      "int inline static dummy_func_2(int a, char b, float c) {" + # This is a sneaky one, inline static is placed AFTER the return value
      "	c += 3.14;" +
      "	b -= 32;" +
      "	return a + (int)(b) + (int)c;" +
      "}" +
      "#endif _NOINCLUDES\n"

    expected =
      "#ifndef _NOINCLUDES\n" +
      "#define _NOINCLUDES\n" +
      "#include \"unity.h\"\n" +
      "#include \"cmock.h\"\n" +
      "#include \"YetAnotherHeader.h\"\n" +
      "\n" +
      "/* Ignore the following warnings since we are copying code */\n" +
      "#if defined(__GNUC__) && !defined(__ICC) && !defined(__TMS470__)\n" +
      "#if __GNUC__ > 4 || (__GNUC__ == 4 && (__GNUC_MINOR__ > 6 || (__GNUC_MINOR__ == 6 && __GNUC_PATCHLEVEL__ > 0)))\n" +
      "#pragma GCC diagnostic push\n" +
      "#endif\n" +
      "#if !defined(__clang__)\n" +
      "#pragma GCC diagnostic ignored \"-Wpragmas\"\n" +
      "#endif\n" +
      "#pragma GCC diagnostic ignored \"-Wunknown-pragmas\"\n" +
      "#pragma GCC diagnostic ignored \"-Wduplicate-decl-specifier\"\n" +
      "#endif\n" +
      "\n" +
      "struct my_struct {\n" +
      "int a;\n" +
      "int b;\n" +
      "int b;\n" +
      "char c;\n" +
      "};\n" +
      "int my_function(int a);\n" +
      "int my_better_function(struct my_struct *s);\n" +
      "int staticinlinebar(struct my_struct *s);\n" +
      "int staticinlinefunc(struct my_struct *s);\n" +
      "int bar(struct my_struct *s);\n" +
      "int inlinestaticfunc(int a);\n" +
      "int StaticInlineFunc(struct my_struct *s);\n" +
      "int StaticInlineBar(struct my_struct *s);\n" +
      "struct staticinlinestruct {\n" +
      "int a;\n" +
      "};\n" +
      "\n" +
      "struct staticinlinestruct fubarstruct(struct my_struct *s);\n" +
      "struct staticinlinestruct inlinefubarfunction(struct my_struct *s);\n" +
      "int fubar(struct my_struct *s);\n" +
      "int stuff(int num);\n" +
      "int dummy_func_2(int a, char b, float c);" +
      "#endif _NOINCLUDES\n"

    assert_equal(expected, @parser.transform_inline_functions(source))
  end

  it "Count number of pairs of braces in function succesfully" do
    source =
      "int foo(struct my_struct *s)\n" +
      "{\n" +
      "    return get_member_a(s) + 42;\n" +
      "}\n"
    complex_source =
      "int bar(struct my_struct *s)\n" +
      "{\n" +
      "\tint a = 6;\n" +
      "\tint res = foo(&(struct my_struct){.nr = a});\n" +
      "\t{\n" +
      "\t\tint a = 5;\n" +
      "\t\tint res = foo(&(struct my_struct){.nr = a});\n" +
      "\t\t{\n" +
      "\t\t\tstruct my_struct a = {.nr = 1};\n" +
      "\t\t}\n" +
      "\t}\n" +
      "\treturn 42;\n" +
      "}\n"

    assert_equal(1, @parser.count_number_of_pairs_of_braces_in_function(source))
    assert_equal(6, @parser.count_number_of_pairs_of_braces_in_function(complex_source))
  end

  it "Count number of pairs of braces returns 0 if bad source is supplied" do
    bad_source_0 =
      "int foo(struct my_struct *s)\n" +
      "{\n" +
      "    return get_member_a(s) + 42;\n" +
      "\n"                      # Missing closing brace
    bad_source_1 =
      "int bar(struct my_struct *s)\n" +
      "{\n" +
      "\tint a = 6;\n" +
      "\tint res = foo(&(struct my_struct){.nr = a});\n" +
      "\t{\n" +
      "\t\tint a = 5;\n" +
      "\t\tint res = foo(&(struct my_struct){.nr = a});\n" +
      "\t\t{\n" +
      "\t\t\n" +                # Missing closing brace
      "\t}\n" +
      "\treturn 42;\n" +
      "}\n"
    bad_source_2 =
      "int foo(struct my_struct *s)\n" +
      "\n"                      # No braces in source

    assert_equal(0, @parser.count_number_of_pairs_of_braces_in_function(bad_source_0))
    assert_equal(0, @parser.count_number_of_pairs_of_braces_in_function(bad_source_1))
    assert_equal(0, @parser.count_number_of_pairs_of_braces_in_function(bad_source_2))
  end

  it "handles parsing multiline functions" do
    source = "int\nLaverneAndShirley(int Lenny,\n                   int Squiggy);\n"
    expected = [{ :var_arg=>nil,
                  :return=> { :type   => "int",
                              :name   => 'cmock_to_return',
                              :ptr?   => false,
                              :const? => false,
                              :const_ptr? => false,
                              :str    => "int cmock_to_return",
                              :void?  => false
                            },
                  :name=>"LaverneAndShirley",
                  :unscoped_name=>"LaverneAndShirley",
                  :namespace=>[],
                  :class=>nil,
                  :modifier=>"",
                  :contains_ptr? => false,
                  :args=>[ {:type=>"int", :name=>"Lenny", :ptr? => false, :const? => false, :const_ptr? => false},
                           {:type=>"int", :name=>"Squiggy", :ptr? => false, :const? => false, :const_ptr? => false}
                         ],
                  :args_string=>"int Lenny, int Squiggy",
                  :args_call=>"Lenny, Squiggy"
               }]
    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "imports C++ differently when asked" do
    source =
    [
      "namespace ns1 {\n",
      "  namespace ns2 {\n",
      "\n",
      "    class cls1 {\n",
      "      public:\n",
      "        int f_header_impl(int a, int b){\n",
      "          return a + b;\n",
      "        }\n",
      "\n",
      "        static void f_void();\n",
      "        static int f_ret_simple();\n",
      "\n",
      "      protected:\n",
      "        static void protected_f_void();\n",
      "\n",
      "      public:\n",
      "      private:\n",
      "        static void private_f_void();\n",
      "    }; // cls1\n",
      "  } // ns2\n",
      "} // ns1\n"
    ].join

    expected =
    [
      "namespace ns1 { namespace ns2 { class cls1 { public: int f_header_impl",
      "static void f_void()",
      "static int f_ret_simple()",
      "protected: static void protected_f_void()",
      "public: private: static void private_f_void()",
      "}",
      "} }"
    ]

    assert_equal(expected, @parser.import_source(source, cpp=true))
    refute_equal(expected, @parser.import_source(source))
  end

  # only so parse_functions does not raise an error
  def dummy_source
    "void dummy(void);"
  end

  # Expected result of above
  def dummy_func
    { :name => "dummy",
      :unscoped_name => "dummy",
      :class => nil,
      :namespace => [],
      :var_arg => nil,
      :args_string => "void",
      :args => [],
      :args_call => "",
      :contains_ptr? => false,
      :modifier => "",
      :return => {
        :type => "void",
        :name => "cmock_to_return",
        :str => "void cmock_to_return",
        :void? => true,
        :ptr? => false,
        :const? => false,
        :const_ptr? => false}}
  end

  # Commonly used example function
  def voidvoid_func(namespace=[], name="Classic_functional")
    { :name => name,
      :unscoped_name => "functional",
      :class => "Classic",
      :namespace => namespace,
      :var_arg => nil,
      :args_string => "void",
      :args => [],
      :args_call => "",
      :contains_ptr? => false,
      :modifier => "",
      :return => {
        :type=>"void",
        :name=>"cmock_to_return",
        :str=>"void cmock_to_return",
        :void? => true,
        :ptr? => false,
        :const? => false,
        :const_ptr? => false}}
  end

  it "parses public C++ functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        public:
          static void functional(void);
      };
    SOURCE

    expected = [dummy_func, voidvoid_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "handles a namespace" do
    source = dummy_source + <<~SOURCE
      namespace ns1 {
        class Classic {
          public:
            static void functional(void);
        };
      } // ns1
    SOURCE

    expected = [dummy_func,
      voidvoid_func(namespace=["ns1"], name="ns1_Classic_functional")]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "handles nested namespaces" do
    source = dummy_source + <<~SOURCE
      namespace ns1 {
        namespace ns2 {
          class Classic {
            public:
              static void functional(void);
          };
        } // ns1
      } // ns1
    SOURCE

    expected = [dummy_func,
      voidvoid_func(namespace=["ns1", "ns2"], name="ns1_ns2_Classic_functional")]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "ignores non-static C++ functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        public:
          void functional(void);
      };
    SOURCE

    expected = [dummy_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "ignores private functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        private:
          static void functional(void);
      };
    SOURCE

    expected = [dummy_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "parses public C++ functions after private functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        private:
          static void ignoreme(void);
        public:
          static void functional(void);
      };
    SOURCE

    expected = [dummy_func, voidvoid_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "ignores protected functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        protected:
          static void functional(void);
      };
    SOURCE

    expected = [dummy_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "parses public C++ functions after protected functions" do
    source = dummy_source + <<~SOURCE
      class Classic {
        protected:
          static void ignoreme(void);
        public:
          static void functional(void);
      };
    SOURCE

    expected = [dummy_func, voidvoid_func]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "parses multiple classes in same file with uniquely named functions" do
    source = dummy_source + <<~SOURCE
      namespace ns1 {
        class Classic {
          public:
            static void functional(void);
        };

        class Classical {
          public:
            static int functionality(int a);
        };
      } // ns1

      class Classy {
        public:
          static int* func(int* a);
      };
    SOURCE

    expected = [dummy_func,
      voidvoid_func(["ns1"], name="ns1_Classic_functional"),
      { :name => "ns1_Classical_functionality",
        :unscoped_name => "functionality",
        :class => "Classical",
        :namespace => ["ns1"],
        :var_arg => nil,
        :args_string => "int a",
        :args => [
          { :ptr? => false,
            :const? => false,
            :const_ptr? => false,
            :name => "a",
            :type => "int"}],
        :args_call => "a",
        :contains_ptr? => false,
        :modifier => "",
        :return => {
          :type=>"int",
          :name=>"cmock_to_return",
          :str=>"int cmock_to_return",
          :void? => false,
          :ptr? => false,
          :const? => false,
          :const_ptr? => false}},
      { :name => "Classy_func",
        :unscoped_name => "func",
        :class => "Classy",
        :namespace => [],
        :var_arg => nil,
        :args_string => "int* a",
        :args => [
          { :ptr? => true,
            :const? => false,
            :const_ptr? => false,
            :name => "a",
            :type => "int*"}],
        :args_call => "a",
        :contains_ptr? => true,
        :modifier => "",
        :return => {
          :type=>"int*",
          :name=>"cmock_to_return",
          :str=>"int* cmock_to_return",
          :void? => false,
          :ptr? => true,
          :const? => false,
          :const_ptr? => false}}]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "handles multiple classes in same file with identically named functions" do
    source = dummy_source + <<~SOURCE
      namespace ns1 {
        class Classic {
          public:
            static void functional(void);
        };

        class Classical {
          public:
            static int functional(int a);
        };
      } // ns1

      class Classy {
        public:
          static int* functional(int* a);
      };
    SOURCE

    expected = [dummy_func,
      voidvoid_func(["ns1"], name="ns1_Classic_functional"),
      { :name => "ns1_Classical_functional",
        :unscoped_name => "functional",
        :class => "Classical",
        :namespace => ["ns1"],
        :var_arg => nil,
        :args_string => "int a",
        :args => [
          { :ptr? => false,
            :const? => false,
            :const_ptr? => false,
            :name => "a",
            :type => "int"}],
        :args_call => "a",
        :contains_ptr? => false,
        :modifier => "",
        :return => {
          :type=>"int",
          :name=>"cmock_to_return",
          :str=>"int cmock_to_return",
          :void? => false,
          :ptr? => false,
          :const? => false,
          :const_ptr? => false}},
      { :name => "Classy_functional",
        :unscoped_name => "functional",
        :class => "Classy",
        :namespace => [],
        :var_arg => nil,
        :args_string => "int* a",
        :args => [
          { :ptr? => true,
            :const? => false,
            :const_ptr? => false,
            :name => "a",
            :type => "int*"}],
        :args_call => "a",
        :contains_ptr? => true,
        :modifier => "",
        :return => {
          :type=>"int*",
          :name=>"cmock_to_return",
          :str=>"int* cmock_to_return",
          :void? => false,
          :ptr? => true,
          :const? => false,
          :const_ptr? => false}}]

    assert_equal(expected, @parser.parse("module", source)[:functions])
  end

  it "Transform inline functions can handle inline function declarations" do
    source =
      "static inline int dummy_func_decl(int a, char b, float c);\n" + # First declaration
      "static inline int dummy_func_decl2(int a, char b, float c)\n\n\n\n\n\n;\n" + # Second declaration with a lot of newlines before the semicolon to mess with the parser
      "static inline int staticinlinefunc(struct my_struct *s)\n" + # 'normal' inline pattern
      "{\n" +
      "    return dummy_func_decl(1, 1, 1);\n" +
      "}\n" +
      "struct my_struct_with_inline_in_it\n" + # struct definition in between to mess with the parser
      "{\n" +
      "    int a;\n" +
      "    char b;\n" +
      "    float inlineb;\n" +
      "};\n" +
      "static inline int dummy_func_decl(int a, char b, float c) {\n" + # Second user pattern
      "	return 42;\n" +
      "}\n" +
      "\n"

    expected =
      "int dummy_func_decl(int a, char b, float c);\n" +
      "int dummy_func_decl2(int a, char b, float c)\n\n\n\n\n\n;\n" + # Second declaration with a lot of newlines until the semicolon to mess with the parser
      "int staticinlinefunc(struct my_struct *s);\n" +
      "struct my_struct_with_inline_in_it\n" +
      "{\n" +
      "    int a;\n" +
      "    char b;\n" +
      "    float inlineb;\n" +
      "};\n" +
      "int dummy_func_decl(int a, char b, float c);\n" +
      "\n"

    @parser.treat_inlines = :include
    assert_equal(expected, @parser.transform_inline_functions(source))
  end

  it "Transform inline functions can handle header with only inline function declarations" do
    source =
      "static inline int dummy_func_decl(int a, char b, float c);\n" +
      "\n"

    expected =
      "int dummy_func_decl(int a, char b, float c);\n" +
      "\n"

    @parser.treat_inlines = :include
    assert_equal(expected, @parser.transform_inline_functions(source))
  end

  it "Transform inline functions takes user provided patterns into account" do
    source =
      "static __inline__ __attribute__ ((always_inline)) uint16_t _somefunc (uint32_t a)\n" +
      "{\n" +
      "    return _someotherfunc (a);\n" +
      "}\n" +
      "static __inline__ uint16_t _somefunc_0  (uint32_t a)\n" +
      "{\n" +
      "    return (uint16_t) a;\n" +
      "}\n" +
      "\n"

    expected =
      "uint16_t _somefunc (uint32_t a);\n" +
      "uint16_t _somefunc_0  (uint32_t a);\n" +
      "\n"

    @parser.treat_inlines = :include
    @parser.inline_function_patterns = ['static __inline__ __attribute__ \(\(always_inline\)\)', 'static __inline__']
    assert_equal(expected, @parser.transform_inline_functions(source))
  end

  it "Transform inline functions limits deleting user macro to actual line/word" do
    source =
      "#if defined (FORCE_INLINE)\n" +
      "#define MY_LIBRARY_INLINE static __inline__ __attribute__ ((always_inline))\n" +
      "#else\n" +
      "#define MY_LIBRARY_INLINE\n" +
      "#endif\n" +
      "#define INLINE static __inline__ __attribute__ ((always_inline))\n" +
      "#define INLINE_TWO \\\nstatic\\\ninline\n" +
      "INLINE uint16_t _somefunc (uint32_t a)\n" +
      "{\n" +
      "    return _someotherfunc (a);\n" +
      "}\n" +
      "static __inline__ uint16_t _somefunc_0  (uint32_t a)\n" +
      "{\n" +
      "    return (uint16_t) a;\n" +
      "}\n" +
      "static __inline__ __attribute__ \(\(always_inline\)\) uint16_t _somefunc_1  (uint32_t a)\n" +
      "{\n" +
      "    return (uint16_t) a;\n" +
      "}\n" +
      "INLINE_TWO uint16_t _somefunc_2(uint32_t a)\n" +
      "{\n" +
      "    return (uint16_t) a;\n" +
      "}\n" +
      "#define INLINE_THREE \\\nstatic\\\ninline"

    expected =
      "#if defined (FORCE_INLINE)\n" +
      "#else\n" +
      "#endif\n" +
      "uint16_t _somefunc (uint32_t a);\n" +
      "uint16_t _somefunc_0  (uint32_t a);\n" +
      "uint16_t _somefunc_1  (uint32_t a);\n" +
      "uint16_t _somefunc_2(uint32_t a);\n"

    @parser.treat_inlines = :include
    @parser.inline_function_patterns = ['MY_LIBRARY_INLINE', 'INLINE_THREE', 'INLINE_TWO', 'INLINE', 'static __inline__ __attribute__ \(\(always_inline\)\)', 'static __inline__']
    assert_equal(expected, @parser.transform_inline_functions(source))
  end

end
