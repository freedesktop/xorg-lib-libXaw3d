/***********************************************************

Copyright (c) 1987, 1988, 1994  X Consortium

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
X CONSORTIUM BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the X Consortium shall not be
used in advertising or otherwise to promote the sale, use or other dealings
in this Software without prior written authorization from the X Consortium.


Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Digital not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif
/*
 * Portions Copyright (c) 1996 Alfredo Kojima
 * Rights, permissions, and disclaimer per the above X Consortium license.
 */

/*
 * This is a copy of Xt/Vendor.c with an additional ClassInitialize
 * procedure to register Xmu resource type converters, and all the
 * monkey business associated with input methods...
 *
 */

/* Make sure all wm properties can make it out of the resource manager */

#include <X11/Xaw3d/Xaw3dP.h>
#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>
#include <X11/VendorP.h>
#include <X11/Xmu/Converters.h>
#include <X11/Xmu/Atoms.h>
#include <X11/Xmu/Editres.h>
#ifdef XAW_INTERNATIONALIZATION
#include <X11/Xmu/ExtAgent.h>
#endif
#ifdef XAW_MULTIPLANE_PIXMAPS
#include <X11/xpm.h>
#include <X11/Xmu/Drawing.h>
#endif

/* The following two headers are for the input method. */
#ifdef XAW_INTERNATIONALIZATION
#include <X11/Xaw3d/VendorEP.h>
#include <X11/Xaw3d/XawImP.h>
#endif


static XtResource resources[] = {
  {XtNinput, XtCInput, XtRBool, sizeof(Bool),
		XtOffsetOf(VendorShellRec, wm.wm_hints.input),
		XtRImmediate, (XtPointer)True}
};

/***************************************************************************
 *
 * Vendor shell class record
 *
 ***************************************************************************/

static void XawVendorShellClassInitialize(void);
static void XawVendorShellInitialize(Widget, Widget, ArgList, Cardinal *);
static Boolean XawVendorShellSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Realize(Widget, Mask *, XSetWindowAttributes *);
static void ChangeManaged(Widget);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
#ifdef XAW_INTERNATIONALIZATION
static void XawVendorShellClassPartInit(WidgetClass);
void XawVendorShellExtResize(Widget);
#endif

#if defined(__UNIXOS2__) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__APPLE__)
/* to fix the EditRes problem because of wrong linker semantics */
extern WidgetClass vendorShellWidgetClass; /* from Xt/Vendor.c */
extern VendorShellClassRec _XawVendorShellClassRec;
void _XawFixupVendorShell(void);

#if defined(__APPLE__)
__attribute__((constructor))
static void __VendorShellHack(void)
{
    vendorShellWidgetClass = (WidgetClass)(&_XawVendorShellClassRec);
    _XawFixupVendorShell();
}
#endif

#if defined(__UNIXOS2__)
unsigned long _DLL_InitTerm(unsigned long mod,unsigned long flag)
{
        switch (flag) {
        case 0: /*called on init*/
                _CRT_init();
                vendorShellWidgetClass = (WidgetClass)(&_XawVendorShellClassRec$
                _XawFixupVendorShell();
                return 1;
        case 1: /*called on exit*/
                return 1;
        default:
                return 0;
        }
}
#endif

#if defined(__CYGWIN__) || defined(__MINGW32__)
int __stdcall DllMain(unsigned long, unsigned long, void *);

int __stdcall
DllMain(unsigned long mod_handle, unsigned long flag, void *routine)
{
  switch (flag)
    {
    case 1: /* DLL_PROCESS_ATTACH - process attach */
      vendorShellWidgetClass = (WidgetClass)(&_XawVendorShellClassRec);
      _XawFixupVendorShell();
      break;
    case 0: /* DLL_PROCESS_DETACH - process detach */
      break;
    }
  return 1;
}
#endif

#define vendorShellClassRec _XawVendorShellClassRec

#endif

#ifdef XAW_INTERNATIONALIZATION
static CompositeClassExtensionRec vendorCompositeExt = {
    /* next_extension     */	NULL,
    /* record_type        */    NULLQUARK,
    /* version            */    XtCompositeExtensionVersion,
    /* record_size        */    sizeof (CompositeClassExtensionRec),
    /* accepts_objects    */    TRUE,
    /* allows_change_managed_set */ FALSE
};
#endif

#define SuperClass (&wmShellClassRec)
externaldef(vendorshellclassrec) VendorShellClassRec vendorShellClassRec = {
  {
    /* superclass	  */	(WidgetClass)SuperClass,
    /* class_name	  */	"VendorShell",
    /* size		  */	sizeof(VendorShellRec),
    /* class_initialize	  */	XawVendorShellClassInitialize,
#ifdef XAW_INTERNATIONALIZATION
    /* class_part_init	  */	XawVendorShellClassPartInit,
#else
    /* class_part_init	  */	NULL,
#endif
    /* Class init'ed ?	  */	FALSE,
    /* initialize         */	XawVendorShellInitialize,
    /* initialize_hook	  */	NULL,
    /* realize		  */	Realize,
    /* actions		  */	NULL,
    /* num_actions	  */	0,
    /* resources	  */	resources,
    /* resource_count	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	FALSE,
    /* compress_exposure  */	TRUE,
    /* compress_enterleave*/	FALSE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */	NULL,
#ifdef XAW_INTERNATIONALIZATION
    /* resize		  */	XawVendorShellExtResize,
#else
    /* resize		  */	XtInheritResize,
#endif
    /* expose		  */	NULL,
    /* set_values	  */	XawVendorShellSetValues,
    /* set_values_hook	  */	NULL,
    /* set_values_almost  */	XtInheritSetValuesAlmost,
    /* get_values_hook	  */	NULL,
    /* accept_focus	  */	NULL,
    /* intrinsics version */	XtVersion,
    /* callback offsets	  */	NULL,
    /* tm_table		  */	NULL,
    /* query_geometry	  */	NULL,
    /* display_accelerator*/	NULL,
    /* extension	  */	NULL
  },{
    /* geometry_manager	  */	GeometryManager,
    /* change_managed	  */	ChangeManaged,
    /* insert_child	  */	XtInheritInsertChild,
    /* delete_child	  */	XtInheritDeleteChild,
#ifdef XAW_INTERNATIONALIZATION
    /* extension	  */	(XtPointer) &vendorCompositeExt
#else
    /* extension	  */	NULL
#endif
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  }
};

#if !defined(__APPLE__)
externaldef(vendorshellwidgetclass) WidgetClass vendorShellWidgetClass =
	(WidgetClass) (&vendorShellClassRec);
#endif

#ifdef XAW_INTERNATIONALIZATION
/***************************************************************************
 *
 * The following section is for the Vendor shell Extension class record
 *
 ***************************************************************************/

static XtResource ext_resources[] = {
  {XtNinputMethod, XtCInputMethod, XtRString, sizeof(String),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.im.input_method),
		XtRString, (XtPointer)NULL},
  {XtNpreeditType, XtCPreeditType, XtRString, sizeof(String),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.im.preedit_type),
		XtRString, (XtPointer)"OverTheSpot,OffTheSpot,Root"},
  {XtNopenIm, XtCOpenIm, XtRBoolean, sizeof(Boolean),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.im.open_im),
		XtRImmediate, (XtPointer)TRUE},
  {XtNsharedIc, XtCSharedIc, XtRBoolean, sizeof(Boolean),
		XtOffsetOf(XawVendorShellExtRec, vendor_ext.ic.shared_ic),
		XtRImmediate, (XtPointer)FALSE}
};

static void XawVendorShellExtClassInitialize(void);
static void XawVendorShellExtInitialize(Widget, Widget, ArgList, Cardinal *);
static void XawVendorShellExtDestroy(Widget);
static Boolean XawVendorShellExtSetValues(Widget, Widget, Widget, ArgList, Cardinal *);

externaldef(vendorshellextclassrec) XawVendorShellExtClassRec
       xawvendorShellExtClassRec = {
  {
    /* superclass	  */	(WidgetClass)&objectClassRec,
    /* class_name	  */	"VendorShellExt",
    /* size		  */	sizeof(XawVendorShellExtRec),
    /* class_initialize	  */	XawVendorShellExtClassInitialize,
    /* class_part_initialize*/	NULL,
    /* Class init'ed ?	  */	FALSE,
    /* initialize	  */	XawVendorShellExtInitialize,
    /* initialize_hook	  */	NULL,
    /* pad		  */	NULL,
    /* pad		  */	NULL,
    /* pad		  */	0,
    /* resources	  */	ext_resources,
    /* resource_count	  */	XtNumber(ext_resources),
    /* xrm_class	  */	NULLQUARK,
    /* pad		  */	FALSE,
    /* pad		  */	FALSE,
    /* pad		  */	FALSE,
    /* pad		  */	FALSE,
    /* destroy		  */	XawVendorShellExtDestroy,
    /* pad		  */	NULL,
    /* pad		  */	NULL,
    /* set_values	  */	XawVendorShellExtSetValues,
    /* set_values_hook	  */	NULL,
    /* pad		  */	NULL,
    /* get_values_hook	  */	NULL,
    /* pad		  */	NULL,
    /* version		  */	XtVersion,
    /* callback_offsets	  */	NULL,
    /* pad		  */	NULL,
    /* pad		  */	NULL,
    /* pad		  */	NULL,
    /* extension	  */	NULL
  },{
    /* extension	  */	NULL
  }
};

externaldef(xawvendorshellwidgetclass) WidgetClass
     xawvendorShellExtWidgetClass = (WidgetClass) (&xawvendorShellExtClassRec);
#endif


/*ARGSUSED*/
static Boolean
XawCvtCompoundTextToString(Display *dpy, XrmValuePtr args, Cardinal *num_args,
                           XrmValue *fromVal, XrmValue *toVal, XtPointer *cvt_data)
{
    XTextProperty prop;
    char **list;
    int count;
    static char *mbs = NULL;
    int len;

    prop.value = (unsigned char *)fromVal->addr;
    prop.encoding = XA_COMPOUND_TEXT(dpy);
    prop.format = 8;
    prop.nitems = fromVal->size;

    if(XmbTextPropertyToTextList(dpy, &prop, &list, &count) < Success) {
	XtAppWarningMsg(XtDisplayToApplicationContext(dpy),
	"converter", "XmbTextPropertyToTextList", "XawError",
	"conversion from CT to MB failed.", NULL, 0);
	return False;
    }
    len = strlen(*list);
    toVal->size = len;
    mbs = XtRealloc(mbs, len + 1); /* keep buffer because no one call free :( */
    strcpy(mbs, *list);
    XFreeStringList(list);
    toVal->addr = (XtPointer)mbs;
    return True;
}

#ifdef XAW_MULTIPLANE_PIXMAPS
#define DONE(type, address) \
	{to->size = sizeof(type); to->addr = (XPointer)address;}

/* ARGSUSED */
static Boolean
_XawCvtStringToPixmap(Display *dpy, XrmValuePtr args, Cardinal *nargs,
                      XrmValuePtr from, XrmValuePtr to, XtPointer *data)
{
    static Pixmap pixmap;
    Window win;
    XpmAttributes attr;
    XpmColorSymbol colors[1];

    if (*nargs != 3)
	XtAppErrorMsg(XtDisplayToApplicationContext(dpy),
		"_XawCvtStringToPixmap", "wrongParameters", "XtToolkitError",
	"_XawCvtStringToPixmap needs screen, colormap, and background_pixel",
		      (String *) NULL, (Cardinal *) NULL);

    if (strcmp(from->addr, "None") == 0)
    {
	pixmap = None;
	DONE(Pixmap, &pixmap);
	return (True);
    }
    if (strcmp(from->addr, "ParentRelative") == 0)
    {
	pixmap = ParentRelative;
	DONE(Pixmap, &pixmap);
	return (True);
    }

    win = RootWindowOfScreen(*((Screen **) args[0].addr));

    attr.colormap = *((Colormap *) args[1].addr);
    attr.closeness = 32768;	/* might help on 8-bpp displays? */
    attr.valuemask = XpmColormap | XpmCloseness;

    colors[0].name = NULL;
    colors[0].value = "none";
    colors[0].pixel = *((Pixel *) args[2].addr);
    attr.colorsymbols = colors;
    attr.numsymbols = 1;
    attr.valuemask |= XpmColorSymbols;

    if (XpmReadFileToPixmap(dpy, win, (String) from->addr,
			    &pixmap, NULL, &attr) != XpmSuccess)
    {
	if ((pixmap = XmuLocateBitmapFile(*((Screen **) args[0].addr),
	      (char *)from->addr, NULL, 0, NULL, NULL, NULL, NULL)) == None)
	{
	    XtDisplayStringConversionWarning(dpy, (String) from->addr,
					     XtRPixmap);
	    return (False);
	}
    }

    if (to->addr == NULL)
	to->addr = (XtPointer) & pixmap;
    else
    {
	if (to->size < sizeof(Pixmap))
	{
	    to->size = sizeof(Pixmap);
	    XtDisplayStringConversionWarning(dpy, (String) from->addr,
					     XtRPixmap);
	    return (False);
	}

	*((Pixmap *) to->addr) = pixmap;
    }
    to->size = sizeof(Pixmap);
    return (True);
}
#endif

static void
XawVendorShellClassInitialize(void)
{
    static XtConvertArgRec screenConvertArg[] = {
        {XtWidgetBaseOffset, (XtPointer) XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)}
    };
#ifdef XAW_MULTIPLANE_PIXMAPS
    static XtConvertArgRec _XawCvtStrToPix[] = {
	{XtWidgetBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.screen),
	     sizeof(Screen *)},
	{XtWidgetBaseOffset, (XtPointer)XtOffsetOf(WidgetRec, core.colormap),
	     sizeof(Colormap)},
	{XtWidgetBaseOffset,
	     (XtPointer)XtOffsetOf(WidgetRec, core.background_pixel),
	     sizeof(Pixel)}
    };
#endif

    XtAddConverter(XtRString, XtRCursor, XmuCvtStringToCursor,
		   screenConvertArg, XtNumber(screenConvertArg));

#ifdef XAW_MULTIPLANE_PIXMAPS
    XtSetTypeConverter(XtRString, XtRBitmap,
		       (XtTypeConverter)_XawCvtStringToPixmap,
		       _XawCvtStrToPix, XtNumber(_XawCvtStrToPix),
		       XtCacheByDisplay, (XtDestructor)NULL);
#else
    XtAddConverter(XtRString, XtRBitmap, XmuCvtStringToBitmap,
		   screenConvertArg, XtNumber(screenConvertArg));
#endif

    XtSetTypeConverter("CompoundText", XtRString, XawCvtCompoundTextToString,
			NULL, 0, XtCacheNone, NULL);
}

#ifdef XAW_INTERNATIONALIZATION
static void
XawVendorShellClassPartInit(WidgetClass class)
{
    CompositeClassExtension ext;
    VendorShellWidgetClass vsclass = (VendorShellWidgetClass) class;

    if ((ext = (CompositeClassExtension)
	    XtGetClassExtension (class,
				 XtOffsetOf(CompositeClassRec,
					    composite_class.extension),
				 NULLQUARK, 1L, (Cardinal) 0)) == NULL) {
	ext = (CompositeClassExtension) XtNew (CompositeClassExtensionRec);
	if (ext != NULL) {
	    ext->next_extension = vsclass->composite_class.extension;
	    ext->record_type = NULLQUARK;
	    ext->version = XtCompositeExtensionVersion;
	    ext->record_size = sizeof (CompositeClassExtensionRec);
	    ext->accepts_objects = TRUE;
	    ext->allows_change_managed_set = FALSE;
	    vsclass->composite_class.extension = (XtPointer) ext;
	}
    }
}
#endif

#if defined(__osf__) || defined(__UNIXOS2__) || defined(__CYGWIN__) || defined(__MINGW32__) || defined(__APPLE__)
/* stupid OSF/1 shared libraries have the wrong semantics */
/* symbols do not get resolved external to the shared library */
void
_XawFixupVendorShell(void)
{
    transientShellWidgetClass->core_class.superclass =
        (WidgetClass) &vendorShellClassRec;
    topLevelShellWidgetClass->core_class.superclass =
        (WidgetClass) &vendorShellClassRec;
}
#endif

/* ARGSUSED */
static void
XawVendorShellInitialize(Widget req, Widget new, ArgList args, Cardinal *num_args)
{
    XtAddEventHandler(new, (EventMask) 0, TRUE, _XEditResCheckMessages, NULL);
#ifdef XAW_INTERNATIONALIZATION
    XtAddEventHandler(new, (EventMask) 0, TRUE, XmuRegisterExternalAgent, NULL);
    XtCreateWidget("shellext", xawvendorShellExtWidgetClass,
		   new, args, *num_args);
#endif
}

/* ARGSUSED */
static Boolean
XawVendorShellSetValues(Widget old, Widget ref, Widget new, ArgList args, Cardinal *num_args)
{
	return FALSE;
}

static void
Realize(Widget wid, Mask *vmask, XSetWindowAttributes *attr)
{
	WidgetClass super = wmShellWidgetClass;

	/* Make my superclass do all the dirty work */

	(*super->core_class.realize) (wid, vmask, attr);
#ifdef XAW_INTERNATIONALIZATION
	_XawImRealize(wid);
#endif
}


#ifdef XAW_INTERNATIONALIZATION
static void
XawVendorShellExtClassInitialize(void)
{
}

/* ARGSUSED */
static void
XawVendorShellExtInitialize(Widget req, Widget new, ArgList args, Cardinal *num_args)
{
    _XawImInitialize(new->core.parent, new);
}

/* ARGSUSED */
static void
XawVendorShellExtDestroy(Widget w)
{
    _XawImDestroy( w->core.parent, w );
}

/* ARGSUSED */
static Boolean
XawVendorShellExtSetValues(Widget old, Widget ref, Widget new, ArgList args, Cardinal *num_args)
{
	return FALSE;
}

void
XawVendorShellExtResize(Widget w)
{
	ShellWidget sw = (ShellWidget) w;
	Widget childwid;
	int i;
	int core_height;

	_XawImResizeVendorShell( w );
	core_height = _XawImGetShellHeight( w );
	for( i = 0; i < sw->composite.num_children; i++ ) {
	    if( XtIsManaged( sw->composite.children[ i ] ) ) {
		childwid = sw->composite.children[ i ];
		XtResizeWidget( childwid, sw->core.width, core_height,
			       childwid->core.border_width );
	    }
	}
}
#endif

/*ARGSUSED*/
static XtGeometryResult
GeometryManager(Widget wid, XtWidgetGeometry *request, XtWidgetGeometry *reply)
{
	ShellWidget shell = (ShellWidget)(wid->core.parent);
	XtWidgetGeometry my_request;

	if(shell->shell.allow_shell_resize == FALSE && XtIsRealized(wid))
		return(XtGeometryNo);

	if (request->request_mode & (CWX | CWY))
	    return(XtGeometryNo);

	/* %%% worry about XtCWQueryOnly */
	my_request.request_mode = 0;
	if (request->request_mode & CWWidth) {
	    my_request.width = request->width;
	    my_request.request_mode |= CWWidth;
	}
	if (request->request_mode & CWHeight) {
	    my_request.height = request->height
#ifdef XAW_INTERNATIONALIZATION
			      + _XawImGetImAreaHeight( wid )
#endif
			      ;
	    my_request.request_mode |= CWHeight;
	}
	if (request->request_mode & CWBorderWidth) {
	    my_request.border_width = request->border_width;
	    my_request.request_mode |= CWBorderWidth;
	}
	if (XtMakeGeometryRequest((Widget)shell, &my_request, NULL)
		== XtGeometryYes) {
	    /* assert: if (request->request_mode & CWWidth) then
	     * 		  shell->core.width == request->width
	     * assert: if (request->request_mode & CWHeight) then
	     * 		  shell->core.height == request->height
	     *
	     * so, whatever the WM sized us to (if the Shell requested
	     * only one of the two) is now the correct child size
	     */

	    wid->core.width = shell->core.width;
	    wid->core.height = shell->core.height;
	    if (request->request_mode & CWBorderWidth) {
		wid->core.x = wid->core.y = -request->border_width;
	    }
#ifdef XAW_INTERNATIONALIZATION
	    _XawImCallVendorShellExtResize(wid);
#endif
	    return XtGeometryYes;
	} else return XtGeometryNo;
}

static void
ChangeManaged(Widget wid)
{
	ShellWidget w = (ShellWidget) wid;
	Widget* childP;
	int i;

	(*SuperClass->composite_class.change_managed)(wid);
	for (i = w->composite.num_children, childP = w->composite.children;
	     i; i--, childP++) {
	    if (XtIsManaged(*childP)) {
		XtSetKeyboardFocus(wid, *childP);
		break;
	    }
	}
}
