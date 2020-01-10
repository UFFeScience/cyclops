package datastructure;

import java.util.NoSuchElementException;

public class OrderedListAsLinkedList extends AbstractSearchableContainer
		implements OrderedList
{
	protected LinkedList linkedList;

	public OrderedListAsLinkedList()
	{
		linkedList = new LinkedList();
	}

	public void insert(Comparable object)
	{
		linkedList.append(object);
		++count;
	}

	public Comparable get(int offset)
	{
		if (offset < 0 || offset >= count)
			throw new IndexOutOfBoundsException();

		LinkedList.Element ptr = linkedList.getHead();
		for (int i = 0; i < offset && ptr != null; ++i)
			ptr = ptr.getNext();
		return (Comparable) ptr.getDatum();
	}

	public boolean isMember(Comparable object)
	{
		for (LinkedList.Element ptr = linkedList.getHead(); ptr != null; ptr = ptr
				.getNext())
		{
			if ((Comparable) ptr.getDatum() == object)
				return true;
		}
		return false;
	}

	public Comparable find(Comparable arg)
	{
		for (LinkedList.Element ptr = linkedList.getHead(); ptr != null; ptr = ptr
				.getNext())
		{
			Comparable object = (Comparable) ptr.getDatum();
			if (object.isEQ(arg))
				return object;
		}
		return null;
	}

	public void withdraw(Comparable object)
	{
		if (count == 0)
			throw new ContainerEmptyException();
		linkedList.extract(object);
		--count;
	}

	protected class MyCursor implements Cursor
	{
		LinkedList.Element element;

		MyCursor(LinkedList.Element element)
		{
			this.element = element;
		}

		public Comparable getDatum()
		{
			return (Comparable) element.getDatum();
		}

		public void insertAfter(Comparable object)
		{
			element.insertAfter(object);
			++count;
		}

		public void withdraw()
		{
			linkedList.extract(element.getDatum());
			--count;
		}

		public void insertBefore(Comparable comparable)
		{
			element.insertBefore(comparable);
			count++;
		}

	}

	public Cursor findPosition(Comparable arg)
	{
		LinkedList.Element ptr;
		for (ptr = linkedList.getHead(); ptr != null; ptr = ptr.getNext())
		{
			Comparable object = (Comparable) ptr.getDatum();
			if (object.isEQ(arg))
				break;
		}
		return new MyCursor(ptr);
	}

	public void purge()
	{
		linkedList = new LinkedList();
		super.count = 0;
	}

	public void accept(Visitor visitor)
	{
		for (LinkedList.Element element = linkedList.getHead(); element != null; element = element
				.getNext())
		{
			visitor.visit((Comparable) element.getDatum());
			if (visitor.isDone())
				return;
		}

	}

	public Enumeration getEnumeration()
	{
		return new Enumeration()
		{

			public boolean hasMoreElements()
			{
				return element != null;
			}

			public Object nextElement()
			{
				if (element == null)
				{
					throw new NoSuchElementException();
				}
				else
				{
					Object obj = element.getDatum();
					element = element.getNext();
					return obj;
				}
			}

			LinkedList.Element element;

			{
				element = linkedList.getHead();
			}
		};
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}

}
