package main;

import java.io.BufferedReader;
import java.io.BufferedWriter;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.io.OutputStreamWriter;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.Vector;

import data.DataFile;
import data.DataStaticFile;
import data.DataTask;
import datastructure.DigraphAsMatrix;
import datastructure.GraphAsMatrix;

/**
 * @author Rodrigo Alves
 * 
 */
public class MainTasksAndFiles {
	public void generateTasksAndFiles(String inputFileName, String outputFileName) {
		GraphAsMatrix graph = null;

		BufferedReader inputFile = null;
		
		BufferedWriter outputFile = null;

		try {
//			System.out.println(inputFileName);
			
			inputFile = new BufferedReader(new InputStreamReader(new FileInputStream(
					new File(inputFileName))));
			
			outputFile = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(
					new File(outputFileName))));

			String line;
			
			// Head section
			
			line = inputFile.readLine();
			String[] parts = line.split(" ");
			
			int numberOfStaticFiles = Integer.decode(parts[0]);
			int numberOfDynamicFiles = Integer.decode(parts[1]);
			int numberOfTasks = Integer.decode(parts[2]);
			
			outputFile.write(line + " 2 100.00 100.00\n");
			
			// Requirement section
			
			outputFile.write("\n");
			outputFile.write("0 1\n");
			outputFile.write("1 1\n");
			
			// File section - static files

			HashMap<String, DataFile> hashFiles = new HashMap<>();
					
			line = inputFile.readLine();  // Blank-line
			outputFile.write(line + "\n");
			line = inputFile.readLine();
			outputFile.write(line + "\n");
			for (int i = 0; i < numberOfStaticFiles; ++i) {
				String[] fileLineParts = line.split(" ");
				Vector<String> vectorOfVms = new Vector<String>();
				for (int j = 3; j < fileLineParts.length; ++j) {
					vectorOfVms.add(fileLineParts[j]);
				}
				DataStaticFile f = new DataStaticFile(fileLineParts[0],
						                              Double.valueOf(fileLineParts[1]), 
						                              Integer.parseInt(fileLineParts[2]), 
						                              vectorOfVms);
				hashFiles.put(f.getName(), f);
				line = inputFile.readLine();
				outputFile.write(line + "\n");
			}

			// File section - dynamic files
			
			for (int i = 0; i < numberOfDynamicFiles; ++i) {
				String[] fileLineParts = line.split(" ");
				DataFile f = new DataFile(fileLineParts[0], Double.valueOf(fileLineParts[1]));
				hashFiles.put(f.getName(), f);
				line = inputFile.readLine();
				outputFile.write(line + "\n");
			}
			
//			System.out.println(vectorOfFiles);
			
			// Files per task section
			
			HashMap<String, DataTask> hashTasks = new HashMap<>();
			
			line = inputFile.readLine();
			outputFile.write(line + " 1 1\n");
			while (!line.isBlank()) {
//				System.out.println(line);
				String[] taskLineParts = line.split(" ");
				String taskId = taskLineParts[0];
				String taskName = taskLineParts[1];
				double time = Double.valueOf(taskLineParts[2]);
				int numberOfInputFiles = Integer.parseInt(taskLineParts[3]);
				int numberOfOutputFiles = Integer.parseInt(taskLineParts[4]);
				Vector<DataFile> myInputFiles = new Vector<DataFile>();
				for (int i = 0; i < numberOfInputFiles; ++i) {
					line = inputFile.readLine();
					outputFile.write(line + "\n");
					myInputFiles.add(hashFiles.get(line));
				}
				Vector<DataFile> myOutputFiles = new Vector<DataFile>();
				for (int i = 0; i < numberOfOutputFiles; ++i) {
					line = inputFile.readLine();
					outputFile.write(line + "\n");
					myOutputFiles.add(hashFiles.get(line));
				}
				
				hashTasks.put(taskId, new DataTask(taskId, 
						                           taskName, 
						                           time, 
						                           myInputFiles, 
						                           myOutputFiles));
				
				line = inputFile.readLine();
				outputFile.write(line + " 1 1\n");
			}
			
			graph = new DigraphAsMatrix(numberOfTasks);
			
			HashMap<String, Integer> dictionaryForTheIDs = new HashMap<>(numberOfTasks);
			HashMap<String, ArrayList<String>> adjMap = new HashMap<>(numberOfTasks);
			
			// Creation of Vertices
			
			int id = 0;
			line = inputFile.readLine();
			outputFile.write(line + "\n");
			while (line != null && !line.isBlank()) {
//				System.out.println(line);
				String[] taskHead = line.split(" ");
				
				String taskId = taskHead[0];
				int numberOfTaskChildren = Integer.decode(taskHead[1]);
				
				graph.addVertex(id, hashTasks.get(taskId));
				dictionaryForTheIDs.put(taskId, id);
				
				ArrayList<String> list = new ArrayList<String>(numberOfTaskChildren);
				for (int i = 0; i < numberOfTaskChildren; ++i) {
					line = inputFile.readLine();
					outputFile.write(line + "\n");
					list.add(line.trim());
				}
				
				adjMap.put(taskId, list);
				
				line = inputFile.readLine();
				outputFile.write((line == null) ? "" : line + "\n");
				++id;
			}
			
			// Creation of Edges
			
			for (String taskId : dictionaryForTheIDs.keySet()) {
//				System.out.println(taskId + ": " + adjMap.get(taskId));
				for (String anotherTaskId : adjMap.get(taskId)) {
					graph.addEdge(dictionaryForTheIDs.get(taskId), dictionaryForTheIDs.get(
							anotherTaskId));
				}
			}
		} catch (IOException e) {
			e.printStackTrace();
		} finally {
			try {
				if (inputFile != null)
					inputFile.close();
				if (outputFile != null)
					outputFile.close();
			} catch (FileNotFoundException e) {
				e.printStackTrace();
			} catch (IOException e) {
				e.printStackTrace();
			}
		}
	}
	
	public void run() {
		File dir = new File("./Input/TasksAndFiles/");
		
		File[] arquivos = dir.listFiles();
		
		for(File a : arquivos) {
			generateTasksAndFiles("Input/TasksAndFiles/" + a.getName(), "Output/TasksAndFiles/" 
					+ a.getName());
		}
	}
	
	public static void main(String[] args) {
		new MainTasksAndFiles().run();
	}
}
