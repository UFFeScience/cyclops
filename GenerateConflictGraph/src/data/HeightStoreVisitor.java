package data;

import java.util.Vector;

import datastructure.MyVisitor;
import datastructure.Vertex;

public class HeightStoreVisitor implements MyVisitor {
	
	Vector<Vector<DataTask>> myHeightStorage;

	public HeightStoreVisitor(Vector<Vector<DataTask>> heightStorage) {
		myHeightStorage = heightStorage;
	}
	
//	public boolean isDone() {
//		return false;
//	}

	@Override
	public void visit(Object object, int height) {
		if (myHeightStorage.size() <= height) {
			myHeightStorage.add(new Vector<DataTask>());
		}
		
		if (!myHeightStorage.get(height).contains(((Vertex) object).getWeight())) {
			myHeightStorage.get(height).add((DataTask) ((Vertex) object).getWeight());
		}
	}
}
