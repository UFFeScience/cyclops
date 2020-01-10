package data;

public class DataFile {

	// Properties
	
	String name;
	
	Double size;
	
	// Constructors
	
	public DataFile(String name, Double size) {
		this.name = name;
		this.size = size;
	}
	
	// Getters
	
	public String getName() {
		return name;
	}
	
	// toString
	public String toString() {
		return name + " " + size;
	}
}
