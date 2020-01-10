package example;

public class Example
{
    
	public static int geometricSeriesSum2(int x, int n)
    {
		int sum = 0;
	
		for (int i = 0; i <= n; ++i)
			sum = sum * x + 1;
		
		return sum;
    }
    
    public static int geometricSeriesSum(int x, int n)
    {
    	int sum = 0;
    	
    	for (int i = 0; i <= n; ++i)
    	{
    	    int prod = 1;
    	    
    	    for (int j = 0; j < i; ++j)
    	    	prod *= x;
    	    
    	    sum += prod;
    	}
    	
    	return sum;
    }
    
	// Euler's constant
    public static double gamma()
    {
    	double result = 0;
    	
    	for (int i = 1; i <= 500000; ++i)
    	    result += 1./i - Math.log ((i + 1.)/i);
    	
    	return result;
    }
    
	public static int findMaximum(int[] a)
    {
    	int result = a[0];
    	
    	for (int i = 1; i < a.length; ++i)
    	    if (a [i] > result)
    		result = a[i];
    	
    	return result;
    }
	
    public static int factorial(int n)
    {
    	if (n == 0)
    	    return 1;
    	else
    	    return n * factorial(n - 1);
    }
    
    public static int horner(int[] a, int n, int x)
    {
    	int result = a[n];
    	
    	for (int i = n - 1; i >= 0; --i)
    	    result = result * x + a[i];
    	
    	return result;
    }
    
    public static int sum(int n)
    {
    	int result = 0;
    	
    	for (int i = 1; i <= n; ++i)
    	    result += i;
    	
    	return result;
    }
}
