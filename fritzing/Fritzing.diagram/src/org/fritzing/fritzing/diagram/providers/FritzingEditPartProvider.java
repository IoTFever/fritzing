/*
 * (c) Fachhochschule Potsdam
 */
package org.fritzing.fritzing.diagram.providers;

import java.lang.ref.WeakReference;

import org.eclipse.gef.EditPart;
import org.eclipse.gef.EditPartFactory;
import org.eclipse.gef.RootEditPart;
import org.eclipse.gmf.runtime.common.core.service.IOperation;
import org.eclipse.gmf.runtime.diagram.ui.editparts.IGraphicalEditPart;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.AbstractEditPartProvider;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.CreateGraphicEditPartOperation;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.CreateRootEditPartOperation;
import org.eclipse.gmf.runtime.diagram.ui.services.editpart.IEditPartOperation;
import org.eclipse.gmf.runtime.notation.Diagram;
import org.eclipse.gmf.runtime.notation.View;
import org.fritzing.fritzing.diagram.edit.parts.FritzingDiagramRootEditPart;
import org.fritzing.fritzing.diagram.edit.parts.FritzingEditPartFactory;
import org.fritzing.fritzing.diagram.edit.parts.SketchEditPart;
import org.fritzing.fritzing.diagram.part.FritzingVisualIDRegistry;

/**
 * @generated
 */
public class FritzingEditPartProvider extends AbstractEditPartProvider {

	/**
	 * @generated
	 */
	private EditPartFactory factory;

	/**
	 * @generated
	 */
	private boolean allowCaching;

	/**
	 * @generated
	 */
	private WeakReference cachedPart;

	/**
	 * @generated
	 */
	private WeakReference cachedView;

	/**
	 * @generated
	 */
	public FritzingEditPartProvider() {
		setFactory(new FritzingEditPartFactory());
		setAllowCaching(true);
	}

	/**
	 * @generated
	 */
	public final EditPartFactory getFactory() {
		return factory;
	}

	/**
	 * @generated
	 */
	protected void setFactory(EditPartFactory factory) {
		this.factory = factory;
	}

	/**
	 * @generated
	 */
	public final boolean isAllowCaching() {
		return allowCaching;
	}

	/**
	 * @generated
	 */
	protected synchronized void setAllowCaching(boolean allowCaching) {
		this.allowCaching = allowCaching;
		if (!allowCaching) {
			cachedPart = null;
			cachedView = null;
		}
	}

	/**
	 * @generated
	 */
	protected IGraphicalEditPart createEditPart(View view) {
		EditPart part = factory.createEditPart(null, view);
		if (part instanceof IGraphicalEditPart) {
			return (IGraphicalEditPart) part;
		}
		return null;
	}

	/**
	 * @generated
	 */
	protected IGraphicalEditPart getCachedPart(View view) {
		if (cachedView != null && cachedView.get() == view) {
			return (IGraphicalEditPart) cachedPart.get();
		}
		return null;
	}

	/**
	 * @generated
	 */
	public synchronized IGraphicalEditPart createGraphicEditPart(View view) {
		if (isAllowCaching()) {
			IGraphicalEditPart part = getCachedPart(view);
			cachedPart = null;
			cachedView = null;
			if (part != null) {
				return part;
			}
		}
		return createEditPart(view);
	}

	/**
	 * @generated NOT
	 */
	public synchronized boolean provides(IOperation operation) {
		if (operation instanceof CreateRootEditPartOperation) {
			// also needed to set Priority of this provider to Medium in plugin.xml
			return true;
		}
		if (operation instanceof CreateGraphicEditPartOperation) {
			View view = ((IEditPartOperation) operation).getView();
			if (!SketchEditPart.MODEL_ID.equals(FritzingVisualIDRegistry
					.getModelID(view))) {
				return false;
			}
			if (isAllowCaching() && getCachedPart(view) != null) {
				return true;
			}
			IGraphicalEditPart part = createEditPart(view);
			if (part != null) {
				if (isAllowCaching()) {
					cachedPart = new WeakReference(part);
					cachedView = new WeakReference(view);
				}
				return true;
			}
		}
		return false;
	}

	/* (non-Javadoc)
	 * @see org.eclipse.gmf.runtime.diagram.ui.services.editpart.AbstractEditPartProvider#createRootEditPart(org.eclipse.gmf.runtime.notation.Diagram)
	 * 
	 * @generated NOT
	 */
	@Override
	public RootEditPart createRootEditPart(Diagram diagram) {
		return new FritzingDiagramRootEditPart(diagram.getMeasurementUnit());
	}
}
