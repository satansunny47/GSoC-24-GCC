bool
extract_types_for_call (gcall *call_stmt, vec<tree> *ret_vector)
{
  tree callee = gimple_call_fndecl (call_stmt);
  if (callee == NULL_TREE)
    {
      return false;
    }

  if (strcmp (IDENTIFIER_POINTER (DECL_NAME (callee)), "__cxa_throw") == 0)
    {
      // Extracting exception type
      tree exception_type_info = gimple_call_arg (call_stmt, 1);
      if (exception_type_info && TREE_CODE (exception_type_info) == ADDR_EXPR)
        {
          exception_type_info = TREE_OPERAND (exception_type_info, 0);
        }
      if (exception_type_info && TREE_CODE (exception_type_info) == VAR_DECL)
        {
          // Converting the typeinfo to a compile-time type
          tree exception_type
              = TREE_TYPE (decl_assembler_name (exception_type_info));
          if (exception_type)
            {
              ret_vector->safe_push (exception_type);
            }
        }
      return true;
    }
  return false;
}