package datastructure;

public class SortedListAsArray extends OrderedListAsArray implements SortedList
{
	public SortedListAsArray(int i)
    {
        super(i);
    }
	
	public void insert(Comparable object)
	{
		if (count == array.length)
			throw new ContainerFullException();
		int i = count;
		while (i > 0 && array[i - 1].isGT(object))
		{
			array[i] = array[i - 1];
			--i;
		}
		array[i] = object;
		++count;
	}

	protected int findOffset(Comparable object)
	{
		int left = 0;
		int right = count - 1;

		while (left <= right)
		{
			int middle = (left + right) / 2;

			if (object.isGT(array[middle]))
				left = middle + 1;
			else if (object.isLT(array[middle]))
				right = middle - 1;
			else
				return middle;
		}
		return -1;
	}

	public Comparable find(Comparable object)
	{
		int offset = findOffset(object);

		if (offset >= 0)
			return array[offset];
		else
			return null;
	}

	public Cursor findPosition(Comparable object)
	{
		return new MyCursor(findOffset(object))
		{
			public void insertAfter(Comparable object)
			{
				throw new InvalidOperationException();
			}

			public void insertBefore(Comparable object)
			{
				throw new InvalidOperationException();
			}
		};
	}

	public void withdraw(Comparable object)
	{
		if (count == 0)
			throw new ContainerEmptyException();

		int offset = findOffset(object);

		if (offset < 0)
			throw new IllegalArgumentException("object not found");

		int i;
		for (i = offset; i < count - 1; ++i)
			array[i] = array[i + 1];
		array[i] = null;
		--count;
	}
}
