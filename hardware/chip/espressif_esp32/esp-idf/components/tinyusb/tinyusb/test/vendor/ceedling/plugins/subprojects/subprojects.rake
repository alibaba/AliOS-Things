

SUBPROJECTS_PATHS.each do |subproj|

  subproj_source     = subproj[:source]
  subproj_include    = subproj[:include]
  subproj_name       = subproj[:name]
  subproj_build_root = subproj[:build_root]
  subproj_build_out  = "#{subproj[:build_root]}/out"
  subproj_build_c    = "#{subproj[:build_root]}/out/c"
  subproj_build_asm  = "#{subproj[:build_root]}/out/asm"
  subproj_directories = [ subproj_build_root, subproj_build_out, subproj_build_c, subproj_build_asm ]

  subproj_directories.each do |subdir|
    directory(subdir)
  end

  CLEAN.include(File.join(subproj_build_root, '*'))
  CLEAN.include(File.join(subproj_build_out, '*'))

  CLOBBER.include(File.join(subproj_build_root, '**/*'))

  # Add a rule for building the actual static library from our object files
  rule(/#{subproj_build_root}#{'.+\\'+EXTENSION_SUBPROJECTS}$/ => [
      proc do |task_name|
        @ceedling[SUBPROJECTS_SYM].list_all_object_files_for_subproject(task_name)
      end
    ]) do |bin_file|
    @ceedling[:generator].generate_executable_file(
      TOOLS_SUBPROJECTS_LINKER,
      SUBPROJECTS_SYM,
      bin_file.prerequisites,
      bin_file.name,
      @ceedling[:file_path_utils].form_test_build_map_filepath(bin_file.name))
  end

  # Add a rule for building object files from assembly files to link into a library
  if (RELEASE_BUILD_USE_ASSEMBLY)
    rule(/#{subproj_build_asm}#{'.+\\'+EXTENSION_OBJECT}$/ => [
        proc do |task_name|
          @ceedling[SUBPROJECTS_SYM].find_library_assembly_file_for_object(task_name)
        end
      ]) do |object|
      @ceedling[SUBPROJECTS_SYM].replace_constant(:COLLECTION_PATHS_SUBPROJECTS, @ceedling[SUBPROJECTS_SYM].find_my_paths(object.source, :asm))
      @ceedling[SUBPROJECTS_SYM].replace_constant(:COLLECTION_DEFINES_SUBPROJECTS, @ceedling[SUBPROJECTS_SYM].find_my_defines(object.source, :asm))
      @ceedling[:generator].generate_object_file(
        TOOLS_SUBPROJECTS_ASSEMBLER,
        OPERATION_ASSEMBLE_SYM,
        SUBPROJECTS_SYM,
        object.source,
        object.name )
    end
  end

  # Add a rule for building object files from C files to link into a library
  rule(/#{subproj_build_c}#{'.+\\'+EXTENSION_OBJECT}$/ => [
      proc do |task_name|
        @ceedling[SUBPROJECTS_SYM].find_library_source_file_for_object(task_name)
      end
    ]) do |object|
    @ceedling[SUBPROJECTS_SYM].replace_constant(:COLLECTION_PATHS_SUBPROJECTS, @ceedling[SUBPROJECTS_SYM].find_my_paths(object.source, :c))
    @ceedling[SUBPROJECTS_SYM].replace_constant(:COLLECTION_DEFINES_SUBPROJECTS, @ceedling[SUBPROJECTS_SYM].find_my_defines(object.source, :c))
    @ceedling[:generator].generate_object_file(
      TOOLS_SUBPROJECTS_COMPILER,
      OPERATION_COMPILE_SYM,
      SUBPROJECTS_SYM,
      object.source,
      object.name,
      @ceedling[:file_path_utils].form_release_build_c_list_filepath( object.name ) )
  end

  # Add the subdirectories involved to our list of those that should be autogenerated
  task :directories => subproj_directories.clone

  # Finally, add the static library to our RELEASE build dependency list
  task RELEASE_SYM => ["#{subproj_build_root}/#{subproj_name}#{EXTENSION_SUBPROJECTS}"] 
end

