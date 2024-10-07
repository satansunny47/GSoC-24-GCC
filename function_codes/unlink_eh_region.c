static void
unlink_eh_region (eh_region region)
{
  eh_region *link;

  // Check if region is root
  if (!region->outer)
    {
      gcc_unreachable ();
      return;
    }

  link = &region->outer->inner;

  while (*link && *link != region)
    {
      link = &(*link)->next_peer;
    }

  // Ensure the region is in the peer chain
  gcc_assert (*link == region);

  *link = region->next_peer;

  region->outer = NULL;
  region->next_peer = NULL;
}