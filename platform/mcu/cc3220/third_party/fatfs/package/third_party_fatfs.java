/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-D21
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class third_party_fatfs
{
    static final String VERS = "@(#) xdc-D21\n";

    static final Proto.Elm $$T_Bool = Proto.Elm.newBool();
    static final Proto.Elm $$T_Num = Proto.Elm.newNum();
    static final Proto.Elm $$T_Str = Proto.Elm.newStr();
    static final Proto.Elm $$T_Obj = Proto.Elm.newObj();

    static final Proto.Fxn $$T_Met = new Proto.Fxn(null, null, 0, -1, false);
    static final Proto.Map $$T_Map = new Proto.Map($$T_Obj);
    static final Proto.Arr $$T_Vec = new Proto.Arr($$T_Obj);

    static final XScriptO $$DEFAULT = Value.DEFAULT;
    static final Object $$UNDEF = Undefined.instance;

    static final Proto.Obj $$Package = (Proto.Obj)Global.get("$$Package");
    static final Proto.Obj $$Module = (Proto.Obj)Global.get("$$Module");
    static final Proto.Obj $$Instance = (Proto.Obj)Global.get("$$Instance");
    static final Proto.Obj $$Params = (Proto.Obj)Global.get("$$Params");

    static final Object $$objFldGet = Global.get("$$objFldGet");
    static final Object $$objFldSet = Global.get("$$objFldSet");
    static final Object $$proxyGet = Global.get("$$proxyGet");
    static final Object $$proxySet = Global.get("$$proxySet");
    static final Object $$delegGet = Global.get("$$delegGet");
    static final Object $$delegSet = Global.get("$$delegSet");

    Scriptable xdcO;
    Session ses;
    Value.Obj om;

    boolean isROV;
    boolean isCFG;

    Proto.Obj pkgP;
    Value.Obj pkgV;

    ArrayList<Object> imports = new ArrayList<Object>();
    ArrayList<Object> loggables = new ArrayList<Object>();
    ArrayList<Object> mcfgs = new ArrayList<Object>();
    ArrayList<Object> icfgs = new ArrayList<Object>();
    ArrayList<String> inherits = new ArrayList<String>();
    ArrayList<Object> proxies = new ArrayList<Object>();
    ArrayList<Object> sizes = new ArrayList<Object>();
    ArrayList<Object> tdefs = new ArrayList<Object>();

    void $$IMPORTS()
    {
        Global.callFxn("loadPackage", xdcO, "ti.sysbios");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("third_party.fatfs.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("third_party.fatfs", new Value.Obj("third_party.fatfs", pkgP));
    }

    void $$SINGLETONS()
    {
        pkgP.init("third_party.fatfs.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "third_party.fatfs"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "third_party/fatfs/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "third_party.fatfs"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "third_party.fatfs"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "third_party.fatfs"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "third_party.fatfs"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "third_party.fatfs"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "third_party.fatfs"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "third_party.fatfs", Value.DEFAULT, false);
        pkgV.bind("$name", "third_party.fatfs");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "third_party.fatfs.");
        pkgV.bind("$vers", Global.newArray());
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['third_party.fatfs'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('third_party.fatfs$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['third_party.fatfs$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['third_party.fatfs$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/fatfs.aem4',\n");
            sb.append("'lib/fatfs.am4g',\n");
            sb.append("'lib/fatfs.arm4',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/fatfs.aem4', {target: 'ti.targets.arm.elf.M4', suffix: 'em4'}],\n");
            sb.append("['lib/fatfs.am4g', {target: 'gnu.targets.arm.M4', suffix: 'm4g'}],\n");
            sb.append("['lib/fatfs.arm4', {target: 'iar.targets.arm.M4', suffix: 'rm4'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("init", pkgV);
        ((Value.Arr)om.findStrict("$packages", "third_party.fatfs")).add(pkgV);
    }

    public void exec( Scriptable xdcO, Session ses )
    {
        this.xdcO = xdcO;
        this.ses = ses;
        om = (Value.Obj)xdcO.get("om", null);

        Object o = om.geto("$name");
        String s = o instanceof String ? (String)o : null;
        isCFG = s != null && s.equals("cfg");
        isROV = s != null && s.equals("rov");

        $$IMPORTS();
        $$OBJECTS();
        if (isROV) {
        }//isROV
        $$SINGLETONS();
        $$INITIALIZATION();
    }
}
