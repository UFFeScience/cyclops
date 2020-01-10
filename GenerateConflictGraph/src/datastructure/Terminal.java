package datastructure;

import java.io.InputStream;
import java.io.PrintStream;

public class Terminal
{
	public static InputStream in;
	public static PrintStream out;
	public static PrintStream err;

	static
	{
		in = System.in;
		out = System.out;
		err = System.err;
	}

	public Terminal()
	{}

	public static void setIn(InputStream inputstream)
	{
		in = inputstream;
	}

	public static void setOut(PrintStream printstream)
	{
		out = printstream;
	}

	public static void setErr(PrintStream printstream)
	{
		err = printstream;
	}
}
