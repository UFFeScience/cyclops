package data;

import java.util.Vector;

public class DataTask {

	// Properties
	
	private String taskId;
	
	private String taskName;
	
	private double time;
	
	private Vector<DataFile> inputFiles;
	
	private Vector<DataFile> outputFiles;
	
	// Constructors
	
	public DataTask(String taskId, 
			        String taskName, 
			        double time, 
			        Vector<DataFile> inputFiles,
			        Vector<DataFile> outputFiles) {
		this.taskId = taskId;
		this.taskName = taskName;
		this.time = time;
		this.inputFiles = inputFiles;
		this.outputFiles = outputFiles;
	}
	
	// Getters
	

	public String getTaskId() {
		return taskId;
	}

	public String getTaskName() {
		return taskName;
	}

	public double getTime() {
		return time;
	}

	public Vector<DataFile> getInputFiles() {
		return inputFiles;
	}

	public Vector<DataFile> getOutputFiles() {
		return outputFiles;
	}

	// Setters
	
	public void setTaskId(String taskId) {
		this.taskId = taskId;
	}

	public void setTaskName(String taskName) {
		this.taskName = taskName;
	}

	public void setTime(double time) {
		this.time = time;
	}

	public void setInputFiles(Vector<DataFile> inputFiles) {
		this.inputFiles = inputFiles;
	}

	public void setOutputFiles(Vector<DataFile> outputFiles) {
		this.outputFiles = outputFiles;
	}
	
	// toString
	
	public String toString() {
		return taskId + " " + taskName + " " + time + " " + inputFiles.size() + " " 
				+ outputFiles.size();
	}
}
