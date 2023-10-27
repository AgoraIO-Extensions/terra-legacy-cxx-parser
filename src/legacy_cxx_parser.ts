import { execSync } from "child_process";
import path from "path";
import { ParseResult, TerraContext } from "@agoraio-extensions/terra-core";
import { LegacyCXXParserConfigs } from "./legacy_cxx_parser_configs";
import * as fs from "fs";

export function runAgoraRtcAstBash(
  terraBuildDir: string,
  language: string,
  includeHeaderDirs: string[],
  customHeaders: string[],
  parseFiles: string[],
  defines: string[],
  legacyFlags: string[],
  outputDir: string | undefined,
  legacyRenders: string[]
) {
  // Find the `@agoraio-extensions/cxx-parser` path.
  let module = "@agoraio-extensions/cxx-parser";
  let cxxParserModulePath = require.resolve(`${module}/package.json`);
  let terraPath = path.join(path.dirname(cxxParserModulePath), "cxx", "terra");
  // <project>/node_modules/@agoraio-extensions/cxx-parser/cxx/cppast_backend/include/system_fake
  let systemInclude = path.join(
    path.dirname(cxxParserModulePath),
    "cxx",
    "cppast_backend",
    "include",
    "system_fake"
  );
  includeHeaderDirs.push(systemInclude);

  let buildDir = path.join(terraBuildDir, "legacy-cxx-parser");
  if (!fs.existsSync(buildDir)) {
    fs.mkdirSync(buildDir);
  }

  let agora_rtc_ast_dir_path = path.join(
    __dirname,
    "..",
    "cxx",
    "cppast_backend"
  );
  console.log("agora_rtc_ast_dir_path: " + agora_rtc_ast_dir_path);

  let build_shell_path = path.join(agora_rtc_ast_dir_path, "build.sh");
  let build_cache_dir_path = path.join(agora_rtc_ast_dir_path, "build");

  let include_header_dirs_arg = includeHeaderDirs.join(",");
  let visit_headers_arg = parseFiles.join(",");

  let bashArgs: string = `--visit-headers=${visit_headers_arg} --include-header-dirs=${include_header_dirs_arg}`;

  bashArgs += ` --language=${language}`;

  let definess = defines.join(",");
  bashArgs += ` --defines-macros=\"${definess}\"`;

  bashArgs += ` --legacy-renders=${legacyRenders.join(",")}`;

  if (outputDir) {
    bashArgs += ` --output-dir=${outputDir}`;
  }

  if (customHeaders) {
    bashArgs += ` --custom-headers=${customHeaders.join(",")}`;
  }

  if (legacyFlags) {
    for (let f of legacyFlags) {
      bashArgs += ` --${f}`;
    }
  }

  let buildScript = `bash ${build_shell_path} ${buildDir} ${terraPath} \"${bashArgs}\"`;
  console.log(`Running command: \n${buildScript}`);

  execSync(buildScript, { encoding: "utf8", stdio: "inherit" });
}

export function LegacyCXXParser(
  terraContext: TerraContext,
  args: any,
  parseResult?: ParseResult
): ParseResult | undefined {
  let legacyCxxParserConfigs = LegacyCXXParserConfigs.resolve(
    terraContext.configDir,
    args
  );

  let parseFiles = (legacyCxxParserConfigs.parseFiles.include ?? []).filter(
    (it) => {
      return !(legacyCxxParserConfigs.parseFiles.exclude ?? []).includes(it);
    }
  );
  runAgoraRtcAstBash(
    terraContext.buildDir,
    legacyCxxParserConfigs.language,
    legacyCxxParserConfigs.includeHeaderDirs,
    legacyCxxParserConfigs.customHeaders ?? [],
    parseFiles,
    legacyCxxParserConfigs.definesMacros ?? [],
    legacyCxxParserConfigs.legacyFlags ?? [],
    terraContext.outputDir,
    legacyCxxParserConfigs.legacyRenders ?? []
  );

  return undefined;
}
