static bool
same_or_derived_type (tree t1, tree t2)
{
  t1 = TYPE_MAIN_VARIANT (t1);
  t2 = TYPE_MAIN_VARIANT (t2);
  if (t1 == t2)
    return true;
  while ((TREE_CODE (t1) == POINTER_TYPE || TREE_CODE (t1) == REFERENCE_TYPE)
         && TREE_CODE (t1) == TREE_CODE (t2))
    {
      t1 = TYPE_MAIN_VARIANT (TREE_TYPE (t1));
      t2 = TYPE_MAIN_VARIANT (TREE_TYPE (t2));
      if (TREE_CODE (t1) == VOID_TYPE)
        return true;
    }
  if (t1 == t2)
    return true;
  if (TREE_CODE (t2) == NULLPTR_TYPE && TREE_CODE (t1) == POINTER_TYPE)
    return true;
  if (!AGGREGATE_TYPE_P (t1) || !AGGREGATE_TYPE_P (t2))
    return false;
  return odr_equivalent_or_derived_p (t1, t2);
}
