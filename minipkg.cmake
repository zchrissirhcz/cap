# Usage:
# minipkg_import(pkg_name)
macro(minipkg_import)

  foreach(pkg_name ${ARGV})
    message(STATUS "importing package: ${pkg_name}")
    set(pkg_imported False)

    while(TRUE)
      # 1. treat as package
      find_package(${pkg_name} QUIET)
      if(${pkg_name}_FOUND)
        message(STATUS "  found pkg: ${pkg_name} (${pkg_name}_DIR: ${${pkg_name}_DIR})")
        
        # list of targets, e.g. OpenCV -> ${OpenCV_LIBRARIES}
        if(DEFINED ${pkg_name}_LIBRARIES)
          foreach(sublib ${${pkg_name}_LIBRARIES})
            add_library(minipkg::${sublib} ALIAS ${sublib})
            message(STATUS "  importing minipkg::${sublib}")
          endforeach()
          set(pkg_imported TRUE)
          break()
        endif()

        # path of single library file, e.g. OpenGL -> ${OPENGL_LIBRARIES}
        string(TOUPPER ${pkg_name} pkg_name_upper)
        if(DEFINED ${pkg_name_upper}_LIBRARIES)
          list(REMOVE_DUPLICATES ${pkg_name_upper}_LIBRARIES)
          # if length of list is 1, treat as single library
          list(LENGTH ${pkg_name_upper}_LIBRARIES num_items)
          if(${num_items} EQUAL 1)
            add_library(minipkg::${pkg_name} SHARED IMPORTED GLOBAL)
            set_target_properties(minipkg::${pkg_name} PROPERTIES
              IMPORTED_LOCATION ${${pkg_name_upper}_LIBRARIES}
            )
            message(STATUS "  importing minipkg::${pkg_name}")
            set(pkg_imported TRUE)
          endif()
          unset(num_items)
          break()
        endif()
        unset(pkg_name_upper)
        
        # single target, e.g. 
        if(TARGET ${pkg_name})
          add_library(minipkg::${pkg_name} ALIAS ${pkg_name})
          message(STATUS "  importing minipkg::${pkg_name} from target ${pkg_name}")
          set(pkg_imported TRUE)
          break()
        endif()

        # single target, remove number suffix, e.g. glfw3 -> glfw
        string(REGEX REPLACE "[0-9]+$" "" pkg_name_no_number "${pkg_name}")  
        if(TARGET ${pkg_name_no_number})
          add_library(minipkg::${pkg_name} ALIAS ${pkg_name_no_number})
          message(STATUS "  importing minipkg::${pkg_name_no_number} from target ${pkg_name_no_number}")
          set(pkg_imported TRUE)
          unset(pkg_name_no_number)
          break()
        endif()

        break()
      endif()

      # 2. treat as library
      find_library(${pkg_name}_location "${pkg_name}")
      set(lib_location ${${pkg_name}_location})
      if(EXISTS ${lib_location})
        message(STATUS "  found library: ${lib_location}")

        if((IS_DIRECTORY "${lib_location}") AND ("${lib_location}" MATCHES ".framework$"))
          add_library(minipkg::${pkg_name} SHARED IMPORTED GLOBAL)
          set_target_properties(minipkg::${pkg_name} PROPERTIES
            IMPORTED_LOCATION ${${pkg_name}_location}
          )
        endif()

        set(pkg_imported TRUE)
        break()
      endif()
      unset(${pkg_name}_location)
      unset(lib_location)

      if(pkg_imported)
        break()
      endif()

      break()
    endwhile()

    if(NOT ${pkg_imported})
      message(FATAL_ERROR "failed to import package: ${pkg_name}")
    endif()

    # cleanup
    unset(pkg_name)
    unset(pkg_imported)
  endforeach()
endmacro()
