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

/**
 * @author Rodrigo Alves
 * 
 */
public class MainClouds {
	public void generateClouds(String inputFileName, String outputFileName) {
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
			outputFile.write(line + "\n");
			
			String[] parts = line.split(" ");
			
			int numberOfProviders = Integer.decode(parts[0]);
			
			// Provider's section
			System.out.println(line);
			line = inputFile.readLine();
			outputFile.write(line + "\n");
			for (int i = 0; i < numberOfProviders; ++i) {
				String[] providerLineParts = line.split(" ");
				System.out.println(line);
//				String idProvider = providerLineParts[0];
//				String providerName = providerLineParts[1];
//				String periodPerHr = providerLineParts[2]; 
//				String maxOfVms = providerLineParts[3];
				int numberOfVms = Integer.parseInt(providerLineParts[4]);
				
				for (int j = 0; j < numberOfVms; ++j) {
					line = inputFile.readLine();
					outputFile.write(line + " 1 1" + "\n");
				}
				line = inputFile.readLine();
				outputFile.write(line + "\n");
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
		File dir = new File("./Input/Clouds/");
		
		File[] arquivos = dir.listFiles();
		
		for(File a : arquivos) {
			generateClouds("Input/Clouds/" + a.getName(), "Output/Clouds/" 
					+ a.getName().replace(".dag", ".scg"));
		}
	}
	
	public static void main(String[] args) {
		new MainClouds().run();
	}
}
