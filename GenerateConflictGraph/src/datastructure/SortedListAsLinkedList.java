package datastructure;

public class SortedListAsLinkedList extends OrderedListAsLinkedList implements
		SortedList
{
	public void insert(Comparable arg)
	{
		LinkedList.Element ptr;
		LinkedList.Element prevPtr = null;

		for (ptr = linkedList.getHead(); ptr != null; ptr = ptr.getNext())
		{
			Comparable object = (Comparable) ptr.getDatum();
			if (object.isGE(arg))
				break;
			prevPtr = ptr;
		}
		if (prevPtr == null)
			linkedList.prepend(arg);
		else
			prevPtr.insertAfter(arg);
		++count;
	}

	protected LinkedList.Element findElement(Comparable comparable)
	{
		for (LinkedList.Element element = super.linkedList.getHead(); element != null; element = element
				.getNext())
		{
			Comparable comparable1 = (Comparable) element.getDatum();
			if (comparable1.isEQ(comparable))
				return element;
		}

		return null;
	}

	public Cursor findPosition(Comparable comparable)
	{
		return new OrderedListAsLinkedList.MyCursor(findElement(comparable))
		{

			public void insertAfter(Comparable comparable1)
			{
				throw new InvalidOperationException();
			}

			public void insertBefore(Comparable comparable1)
			{
				throw new InvalidOperationException();
			}

		};
	}

}
