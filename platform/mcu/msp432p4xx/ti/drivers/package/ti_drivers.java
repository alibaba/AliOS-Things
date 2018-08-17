/*
 *  Do not modify this file; it is automatically 
 *  generated and any modifications will be overwritten.
 *
 * @(#) xdc-E05
 */
import java.util.*;
import org.mozilla.javascript.*;
import xdc.services.intern.xsr.*;
import xdc.services.spec.Session;

public class ti_drivers
{
    static final String VERS = "@(#) xdc-E05\n";

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
        Global.callFxn("loadPackage", xdcO, "ti.dpl");
        Global.callFxn("loadPackage", xdcO, "xdc");
        Global.callFxn("loadPackage", xdcO, "xdc.corevers");
        Global.callFxn("loadPackage", xdcO, "xdc.runtime");
    }

    void $$OBJECTS()
    {
        pkgP = (Proto.Obj)om.bind("ti.drivers.Package", new Proto.Obj());
        pkgV = (Value.Obj)om.bind("ti.drivers", new Value.Obj("ti.drivers", pkgP));
    }

    void Config$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.drivers.Config.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.drivers.Config", new Value.Obj("ti.drivers.Config", po));
        pkgV.bind("Config", vo);
        // decls 
        om.bind("ti.drivers.Config.LibType", new Proto.Enm("ti.drivers.Config.LibType"));
        om.bind("ti.drivers.Config.RFDriverMode", new Proto.Enm("ti.drivers.Config.RFDriverMode"));
    }

    void Power$$OBJECTS()
    {
        Proto.Obj po, spo;
        Value.Obj vo;

        po = (Proto.Obj)om.bind("ti.drivers.Power.Module", new Proto.Obj());
        vo = (Value.Obj)om.bind("ti.drivers.Power", new Value.Obj("ti.drivers.Power", po));
        pkgV.bind("Power", vo);
        // decls 
    }

    void Config$$CONSTS()
    {
        // module Config
        om.bind("ti.drivers.Config.LibType_Instrumented", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.drivers.Config.LibType", "ti.drivers"), "ti.drivers.Config.LibType_Instrumented", 0));
        om.bind("ti.drivers.Config.LibType_NonInstrumented", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.drivers.Config.LibType", "ti.drivers"), "ti.drivers.Config.LibType_NonInstrumented", 1));
        om.bind("ti.drivers.Config.RF_MultiMode", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.drivers.Config.RFDriverMode", "ti.drivers"), "ti.drivers.Config.RF_MultiMode", 0));
        om.bind("ti.drivers.Config.RF_SingleMode", xdc.services.intern.xsr.Enum.make((Proto.Enm)om.findStrict("ti.drivers.Config.RFDriverMode", "ti.drivers"), "ti.drivers.Config.RF_SingleMode", 1));
    }

    void Power$$CONSTS()
    {
        // module Power
    }

    void Config$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Power$$CREATES()
    {
        Proto.Fxn fxn;
        StringBuilder sb;

    }

    void Config$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Power$$FUNCTIONS()
    {
        Proto.Fxn fxn;

    }

    void Config$$SIZES()
    {
    }

    void Power$$SIZES()
    {
        Proto.Str so;
        Object fxn;

    }

    void Config$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/drivers/Config.xs");
        om.bind("ti.drivers.Config$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.drivers.Config.Module", "ti.drivers");
        po.init("ti.drivers.Config.Module", $$Module);
                po.addFld("$hostonly", $$T_Num, 1, "r");
        po.addFld("libType", (Proto)om.findStrict("ti.drivers.Config.LibType", "ti.drivers"), om.find("ti.drivers.Config.LibType_NonInstrumented"), "wh");
        po.addFld("rfDriverMode", (Proto)om.findStrict("ti.drivers.Config.RFDriverMode", "ti.drivers"), om.find("ti.drivers.Config.RF_SingleMode"), "wh");
        po.addFld("supportsRFDriver", $$T_Bool, false, "wh");
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.drivers.Config$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.drivers.Config$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.drivers.Config$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Power$$TYPES()
    {
        Scriptable cap;
        Proto.Obj po;
        Proto.Str ps;
        Proto.Typedef pt;
        Object fxn;

        cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/drivers/Power.xs");
        om.bind("ti.drivers.Power$$capsule", cap);
        po = (Proto.Obj)om.findStrict("ti.drivers.Power.Module", "ti.drivers");
        po.init("ti.drivers.Power.Module", om.findStrict("xdc.runtime.IModule.Module", "ti.drivers"));
                po.addFld("$hostonly", $$T_Num, 0, "r");
        if (isCFG) {
        }//isCFG
        fxn = Global.get(cap, "module$use");
        if (fxn != null) om.bind("ti.drivers.Power$$module$use", true);
        if (fxn != null) po.addFxn("module$use", $$T_Met, fxn);
        fxn = Global.get(cap, "module$meta$init");
        if (fxn != null) om.bind("ti.drivers.Power$$module$meta$init", true);
        if (fxn != null) po.addFxn("module$meta$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$static$init");
        if (fxn != null) om.bind("ti.drivers.Power$$module$static$init", true);
        if (fxn != null) po.addFxn("module$static$init", $$T_Met, fxn);
        fxn = Global.get(cap, "module$validate");
        if (fxn != null) om.bind("ti.drivers.Power$$module$validate", true);
        if (fxn != null) po.addFxn("module$validate", $$T_Met, fxn);
    }

    void Config$$ROV()
    {
    }

    void Power$$ROV()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.drivers.Power", "ti.drivers");
    }

    void $$SINGLETONS()
    {
        pkgP.init("ti.drivers.Package", (Proto.Obj)om.findStrict("xdc.IPackage.Module", "ti.drivers"));
        Scriptable cap = (Scriptable)Global.callFxn("loadCapsule", xdcO, "ti/drivers/package.xs");
        om.bind("xdc.IPackage$$capsule", cap);
        Object fxn;
                fxn = Global.get(cap, "init");
                if (fxn != null) pkgP.addFxn("init", (Proto.Fxn)om.findStrict("xdc.IPackage$$init", "ti.drivers"), fxn);
                fxn = Global.get(cap, "close");
                if (fxn != null) pkgP.addFxn("close", (Proto.Fxn)om.findStrict("xdc.IPackage$$close", "ti.drivers"), fxn);
                fxn = Global.get(cap, "validate");
                if (fxn != null) pkgP.addFxn("validate", (Proto.Fxn)om.findStrict("xdc.IPackage$$validate", "ti.drivers"), fxn);
                fxn = Global.get(cap, "exit");
                if (fxn != null) pkgP.addFxn("exit", (Proto.Fxn)om.findStrict("xdc.IPackage$$exit", "ti.drivers"), fxn);
                fxn = Global.get(cap, "getLibs");
                if (fxn != null) pkgP.addFxn("getLibs", (Proto.Fxn)om.findStrict("xdc.IPackage$$getLibs", "ti.drivers"), fxn);
                fxn = Global.get(cap, "getSects");
                if (fxn != null) pkgP.addFxn("getSects", (Proto.Fxn)om.findStrict("xdc.IPackage$$getSects", "ti.drivers"), fxn);
        pkgP.bind("$capsule", cap);
        pkgV.init2(pkgP, "ti.drivers", Value.DEFAULT, false);
        pkgV.bind("$name", "ti.drivers");
        pkgV.bind("$category", "Package");
        pkgV.bind("$$qn", "ti.drivers.");
        pkgV.bind("$vers", Global.newArray(1, 0, 0));
        Value.Map atmap = (Value.Map)pkgV.getv("$attr");
        atmap.seal("length");
        imports.clear();
        imports.add(Global.newArray("ti.sysbios", Global.newArray()));
        imports.add(Global.newArray("ti.dpl", Global.newArray()));
        pkgV.bind("$imports", imports);
        StringBuilder sb = new StringBuilder();
        sb.append("var pkg = xdc.om['ti.drivers'];\n");
        sb.append("if (pkg.$vers.length >= 3) {\n");
            sb.append("pkg.$vers.push(Packages.xdc.services.global.Vers.getDate(xdc.csd() + '/..'));\n");
        sb.append("}\n");
        sb.append("if ('ti.drivers$$stat$base' in xdc.om) {\n");
            sb.append("pkg.packageBase = xdc.om['ti.drivers$$stat$base'];\n");
            sb.append("pkg.packageRepository = xdc.om['ti.drivers$$stat$root'];\n");
        sb.append("}\n");
        sb.append("pkg.build.libraries = [\n");
            sb.append("'lib/drivers_msp432p4xx.aem4f',\n");
            sb.append("'lib/drivers_msp432p4xx.am4fg',\n");
            sb.append("'lib/drivers_msp432p4xx.arm4f',\n");
            sb.append("'lib/drivers_msp432p401x.aem4f',\n");
            sb.append("'lib/drivers_msp432p401x.am4fg',\n");
            sb.append("'lib/drivers_msp432p401x.arm4f',\n");
            sb.append("'lib/drivers_msp432p4x1xi.aem4f',\n");
            sb.append("'lib/drivers_msp432p4x1xi.am4fg',\n");
            sb.append("'lib/drivers_msp432p4x1xi.arm4f',\n");
            sb.append("'lib/drivers_msp432p4x1xt.aem4f',\n");
            sb.append("'lib/drivers_msp432p4x1xt.am4fg',\n");
            sb.append("'lib/drivers_msp432p4x1xt.arm4f',\n");
        sb.append("];\n");
        sb.append("pkg.build.libDesc = [\n");
            sb.append("['lib/drivers_msp432p4xx.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/drivers_msp432p4xx.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/drivers_msp432p4xx.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
            sb.append("['lib/drivers_msp432p401x.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/drivers_msp432p401x.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/drivers_msp432p401x.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
            sb.append("['lib/drivers_msp432p4x1xi.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/drivers_msp432p4x1xi.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/drivers_msp432p4x1xi.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
            sb.append("['lib/drivers_msp432p4x1xt.aem4f', {target: 'ti.targets.arm.elf.M4F', suffix: 'em4f'}],\n");
            sb.append("['lib/drivers_msp432p4x1xt.am4fg', {target: 'gnu.targets.arm.M4F', suffix: 'm4fg'}],\n");
            sb.append("['lib/drivers_msp432p4x1xt.arm4f', {target: 'iar.targets.arm.M4F', suffix: 'rm4f'}],\n");
        sb.append("];\n");
        Global.eval(sb.toString());
    }

    void Config$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.drivers.Config", "ti.drivers");
        po = (Proto.Obj)om.findStrict("ti.drivers.Config.Module", "ti.drivers");
        vo.init2(po, "ti.drivers.Config", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.drivers.Config$$capsule", "ti.drivers"));
        vo.bind("$package", om.findStrict("ti.drivers", "ti.drivers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        vo.bind("LibType", om.findStrict("ti.drivers.Config.LibType", "ti.drivers"));
        vo.bind("RFDriverMode", om.findStrict("ti.drivers.Config.RFDriverMode", "ti.drivers"));
        vo.bind("LibType_Instrumented", om.findStrict("ti.drivers.Config.LibType_Instrumented", "ti.drivers"));
        vo.bind("LibType_NonInstrumented", om.findStrict("ti.drivers.Config.LibType_NonInstrumented", "ti.drivers"));
        vo.bind("RF_MultiMode", om.findStrict("ti.drivers.Config.RF_MultiMode", "ti.drivers"));
        vo.bind("RF_SingleMode", om.findStrict("ti.drivers.Config.RF_SingleMode", "ti.drivers"));
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.drivers")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 1);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 0);
        Proto.Str ps = (Proto.Str)vo.find("Module_State");
        if (ps != null) vo.bind("$object", ps.newInstance());
        vo.bind("$$meta_iobj", om.has("ti.drivers.Config$$instance$static$init", null) ? 1 : 0);
        vo.bind("$$fxntab", Global.newArray());
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.seal("length");
        pkgV.bind("Config", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Config");
    }

    void Power$$SINGLETONS()
    {
        Proto.Obj po;
        Value.Obj vo;

        vo = (Value.Obj)om.findStrict("ti.drivers.Power", "ti.drivers");
        po = (Proto.Obj)om.findStrict("ti.drivers.Power.Module", "ti.drivers");
        vo.init2(po, "ti.drivers.Power", $$DEFAULT, false);
        vo.bind("Module", po);
        vo.bind("$category", "Module");
        vo.bind("$capsule", om.findStrict("ti.drivers.Power$$capsule", "ti.drivers"));
        vo.bind("$package", om.findStrict("ti.drivers", "ti.drivers"));
        tdefs.clear();
        proxies.clear();
        mcfgs.clear();
        icfgs.clear();
        inherits.clear();
        mcfgs.add("Module__diagsEnabled");
        icfgs.add("Module__diagsEnabled");
        mcfgs.add("Module__diagsIncluded");
        icfgs.add("Module__diagsIncluded");
        mcfgs.add("Module__diagsMask");
        icfgs.add("Module__diagsMask");
        mcfgs.add("Module__gateObj");
        icfgs.add("Module__gateObj");
        mcfgs.add("Module__gatePrms");
        icfgs.add("Module__gatePrms");
        mcfgs.add("Module__id");
        icfgs.add("Module__id");
        mcfgs.add("Module__loggerDefined");
        icfgs.add("Module__loggerDefined");
        mcfgs.add("Module__loggerObj");
        icfgs.add("Module__loggerObj");
        mcfgs.add("Module__loggerFxn0");
        icfgs.add("Module__loggerFxn0");
        mcfgs.add("Module__loggerFxn1");
        icfgs.add("Module__loggerFxn1");
        mcfgs.add("Module__loggerFxn2");
        icfgs.add("Module__loggerFxn2");
        mcfgs.add("Module__loggerFxn4");
        icfgs.add("Module__loggerFxn4");
        mcfgs.add("Module__loggerFxn8");
        icfgs.add("Module__loggerFxn8");
        mcfgs.add("Object__count");
        icfgs.add("Object__count");
        mcfgs.add("Object__heap");
        icfgs.add("Object__heap");
        mcfgs.add("Object__sizeof");
        icfgs.add("Object__sizeof");
        mcfgs.add("Object__table");
        icfgs.add("Object__table");
        vo.bind("$$tdefs", Global.newArray(tdefs.toArray()));
        vo.bind("$$proxies", Global.newArray(proxies.toArray()));
        vo.bind("$$mcfgs", Global.newArray(mcfgs.toArray()));
        vo.bind("$$icfgs", Global.newArray(icfgs.toArray()));
        inherits.add("xdc.runtime");
        vo.bind("$$inherits", Global.newArray(inherits.toArray()));
        ((Value.Arr)pkgV.getv("$modules")).add(vo);
        ((Value.Arr)om.findStrict("$modules", "ti.drivers")).add(vo);
        vo.bind("$$instflag", 0);
        vo.bind("$$iobjflag", 0);
        vo.bind("$$sizeflag", 1);
        vo.bind("$$dlgflag", 0);
        vo.bind("$$iflag", 0);
        vo.bind("$$romcfgs", "|");
        vo.bind("$$nortsflag", 1);
        if (isCFG) {
            Proto.Str ps = (Proto.Str)vo.find("Module_State");
            if (ps != null) vo.bind("$object", ps.newInstance());
            vo.bind("$$meta_iobj", 1);
        }//isCFG
        vo.bind("$$fxntab", Global.newArray("ti_drivers_Power_Module__startupDone__E"));
        vo.bind("$$logEvtCfgs", Global.newArray());
        vo.bind("$$errorDescCfgs", Global.newArray());
        vo.bind("$$assertDescCfgs", Global.newArray());
        Value.Map atmap = (Value.Map)vo.getv("$attr");
        atmap.setElem("", true);
        atmap.setElem("", "");
        atmap.seal("length");
        if (isCFG) {
            vo.put("common$", vo, Global.get((Proto.Obj)om.find("xdc.runtime.Defaults.Module"), "noRuntimeCommon$"));
            ((Value.Obj)vo.geto("common$")).seal(null);
        }//isCFG
        vo.bind("MODULE_STARTUP$", 0);
        vo.bind("PROXY$", 0);
        loggables.clear();
        vo.bind("$$loggables", loggables.toArray());
        pkgV.bind("Power", vo);
        ((Value.Arr)pkgV.getv("$unitNames")).add("Power");
    }

    void $$INITIALIZATION()
    {
        Value.Obj vo;

        if (isCFG) {
        }//isCFG
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.drivers.Config", "ti.drivers"));
        Global.callFxn("module$meta$init", (Scriptable)om.findStrict("ti.drivers.Power", "ti.drivers"));
        Global.callFxn("init", pkgV);
        ((Value.Obj)om.getv("ti.drivers.Config")).bless();
        ((Value.Obj)om.getv("ti.drivers.Power")).bless();
        ((Value.Arr)om.findStrict("$packages", "ti.drivers")).add(pkgV);
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
        Config$$OBJECTS();
        Power$$OBJECTS();
        Config$$CONSTS();
        Power$$CONSTS();
        Config$$CREATES();
        Power$$CREATES();
        Config$$FUNCTIONS();
        Power$$FUNCTIONS();
        Config$$SIZES();
        Power$$SIZES();
        Config$$TYPES();
        Power$$TYPES();
        if (isROV) {
            Config$$ROV();
            Power$$ROV();
        }//isROV
        $$SINGLETONS();
        Config$$SINGLETONS();
        Power$$SINGLETONS();
        $$INITIALIZATION();
    }
}
