package data;

import datastructure.Visitor;

public class PrintingVisitor implements Visitor {

	public void visit(Object object) {
		System.out.println(object);
	}

	public boolean isDone() {
		return false;
	}
}
