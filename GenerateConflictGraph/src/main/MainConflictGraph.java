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
import data.HeightStoreVisitor;
import datastructure.DigraphAsMatrix;
import datastructure.Edge;
import datastructure.Enumeration;
import datastructure.GraphAsMatrix;

/**
 * @author Rodrigo Alves
 * 
 */
public class MainConflictGraph {
	public void generateConflictGraph(String inputFileName, String outputFileName) {
		GraphAsMatrix graph = null;

		BufferedReader inputFile = null;
		
		BufferedWriter outputFile = null;

		try {
//			System.out.println(inputFileName);
			
			inputFile = new BufferedReader(new InputStreamReader(new FileInputStream(
					new File(inputFileName))));

			String line;
			
			// Head section
			
			line = inputFile.readLine();
			String[] parts = line.split(" ");
			
			int numberOfStaticFiles = Integer.decode(parts[0]);
			int numberOfDynamicFiles = Integer.decode(parts[1]);
			int numberOfTasks = Integer.decode(parts[2]);
			
			HashMap<String, DataFile> hashFiles = new HashMap<>();
			
			// File section - static files
			line = inputFile.readLine();  // Blank-line
			line = inputFile.readLine();
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
			}

			// File section - dynamic files
			
			for (int i = 0; i < numberOfDynamicFiles; ++i) {
				String[] fileLineParts = line.split(" ");
				DataFile f = new DataFile(fileLineParts[0], Double.valueOf(fileLineParts[1]));
				hashFiles.put(f.getName(), f);
				line = inputFile.readLine();
			}
			
//			System.out.println(vectorOfFiles);
			
			// Files per task section
			
			HashMap<String, DataTask> hashTasks = new HashMap<>();
			
			line = inputFile.readLine();
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
					myInputFiles.add(hashFiles.get(line));
				}
				Vector<DataFile> myOutputFiles = new Vector<DataFile>();
				for (int i = 0; i < numberOfOutputFiles; ++i) {
					line = inputFile.readLine();
					myOutputFiles.add(hashFiles.get(line));
				}
				
				hashTasks.put(taskId, new DataTask(taskId, 
						                           taskName, 
						                           time, 
						                           myInputFiles, 
						                           myOutputFiles));
				
				line = inputFile.readLine();
			}
			
			graph = new DigraphAsMatrix(numberOfTasks);
			
			HashMap<String, Integer> dictionaryForTheIDs = new HashMap<>(numberOfTasks);
			HashMap<String, ArrayList<String>> adjMap = new HashMap<>(numberOfTasks);
			
			// Creation of Vertices
			
			int id = 0;
			line = inputFile.readLine();
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
					list.add(line.trim());
				}
				
				adjMap.put(taskId, list);
				
				line = inputFile.readLine();
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

//			System.out.println("\nGraph: \n");
//			System.out.println(graph);

			// Write in the output file with extension .scg (security conflict graph)
			outputFile = new BufferedWriter(new OutputStreamWriter(new FileOutputStream(
					new File(outputFileName))));
			
			Enumeration enumEdges = graph.getEdges();
			while (enumEdges.hasMoreElements()) {
				Edge e = (Edge) enumEdges.nextElement();
				
				DataTask u = (DataTask) e.getV0().getWeight();
				DataTask v = (DataTask) e.getV1().getWeight();
				
				for (DataFile dfOfU : u.getOutputFiles()) {
					for (DataFile dfOfV : v.getOutputFiles()) {
						outputFile.write(dfOfU.getName() + " " + dfOfV.getName() + " " + "0\n");
					}
				}
			}
			
			Vector<Vector<DataTask>> heightStorage = new Vector<>();
			
//			System.out.println(heightStorage);
			
			graph.depthFirstTraversal(new HeightStoreVisitor(heightStorage), 0);
			
//			System.out.println(heightStorage);
//			System.out.println(heightStorage.size());
			
			for (int i = 0; i < heightStorage.size(); ++i) {
				if (heightStorage.get(i).size() > 1) {
					for (int j = 0; j < heightStorage.get(i).size() - 1; ++j) {
						for (int k = j + 1; k < heightStorage.get(i).size(); ++k) {

							DataTask u = heightStorage.get(i).get(j);
							DataTask v = heightStorage.get(i).get(k);
							
							for (DataFile dfOfU : u.getOutputFiles()) {
								for (DataFile dfOfV : v.getOutputFiles()) {
									outputFile.write(dfOfU.getName() + " " + dfOfV.getName() + " " 
											+ "1\n");
								}
							}
						}
					}
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
			generateConflictGraph("Input/TasksAndFiles/" + a.getName(), "Output/ConflictGraph/" 
					+ a.getName().replace(".dag", ".scg"));
		}
	}
	
	public static void main(String[] args) {
		new MainConflictGraph().run();
	}
}
