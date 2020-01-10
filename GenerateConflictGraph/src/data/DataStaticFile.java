package data;

import java.util.Vector;

public class DataStaticFile extends DataFile {

	// Properties
	
	int numberOfCopies;
	
	Vector<String> virtualMachines;
	
	// Constructors
	
	public DataStaticFile(String name, 
			              Double size, 
			              int numberOfCopies, 
			              Vector<String> virtualMachines) {
		super(name, size);
		this.numberOfCopies = numberOfCopies;
		this.virtualMachines = virtualMachines;
	}
	
	// Methods
	
	// toString
	public String toString() {
		return super.toString() + " " + numberOfCopies + " " + virtualMachines;
	}
}
