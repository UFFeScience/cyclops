package datastructure;

import java.util.NoSuchElementException;

public class ChainedHashTable extends AbstractHashTable
{
	protected LinkedList[] array;

	public ChainedHashTable(int length)
	{
		array = new LinkedList[length];
		for (int i = 0; i < length; ++i)
			array[i] = new LinkedList();
	}

	public int getLength()
	{
		return array.length;
	}

	public void purge()
	{
		for (int i = 0; i < getLength(); ++i)
			array[i].purge();
		count = 0;
	}

	public void insert(Comparable object)
	{
		array[h(object)].append(object);
		++count;
	}

	public void withdraw(Comparable object)
	{
		array[h(object)].extract(object);
		--count;
	}

	public Comparable find(Comparable object)
	{
		for (LinkedList.Element ptr = array[h(object)].getHead(); ptr != null; ptr = ptr
				.getNext())
		{
			Comparable datum = (Comparable) ptr.getDatum();
			if (object.isEQ(datum))
				return datum;
		}
		return null;
	}

	public boolean isMember(Comparable comparable)
	{
		for (LinkedList.Element element = array[h(comparable)].getHead(); element != null; element = element
				.getNext())
		{
			Comparable comparable1 = (Comparable) element.getDatum();
			if (comparable == comparable1)
				return true;
		}

		return false;
	}

	public void accept(Visitor visitor)
	{
		for (int i = 0; i < getLength(); i++)
		{
			for (LinkedList.Element element = array[i].getHead(); element != null; element = element
					.getNext())
			{
				visitor.visit((Comparable) element.getDatum());
				if (visitor.isDone())
					return;
			}

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
					throw new NoSuchElementException();
				Object obj = element.getDatum();
				element = element.getNext();
				if (element == null)
					for (offset++; offset < getLength(); offset++)
					{
						element = array[offset].getHead();
						if (element != null)
							break;
					}

				return obj;
			}

			protected LinkedList.Element element;
			protected int offset;

			{
				element = null;
				for (offset = 0; offset < getLength(); offset++)
				{
					element = array[offset].getHead();
					if (element != null)
						break;
				}

			}
		};
	}

	protected int compareTo(Comparable comparable)
	{
		throw new MethodNotImplemented();
	}
}
