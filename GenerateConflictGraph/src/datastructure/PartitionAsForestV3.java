package datastructure;

public class PartitionAsForestV3 extends PartitionAsForestV2
{
	public PartitionAsForestV3(int i)
	{
		super(i);
	}

	public void join(Set s, Set t)
	{
		PartitionTree p = (PartitionTree) s;
		PartitionTree q = (PartitionTree) t;
		checkArguments(p, q);
		if (p.rank > q.rank)
			q.parent = p;
		else
		{
			p.parent = q;
			if (p.rank == q.rank)
				q.rank += 1;
		}
		--count;
	}
}
