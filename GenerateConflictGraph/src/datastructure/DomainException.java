package datastructure;

import java.io.Serializable;

public class DomainException extends RuntimeException implements Serializable
{
	private static final long serialVersionUID = 1L;

	public DomainException(String msg)
	{
		super(msg);
	}
	
	public DomainException()
	{
		super();
	}
}
