import { execSync } from "child_process";
import path from "path";
import {
  ParseResult,
  Parser,
  TerraContext,
  requireModule,
  resolveModulePath,
} from "@agoraio-extensions/terra-core";
import { LegacyCXXParserConfigs } from "./legacy_cxx_parser_configs";

export function runAgoraRtcAstBash(
  language: string,
  includeHeaderDirs: string[],
  customHeaders: string[],
  parseFiles: string[],
  defines: string[],
  legacyFlags: string[],
  outputDir: string | undefined,
  legacyRenders: string[]
) {
  // Always find self package in node_modules
  let selfModulePath = resolveModulePath("@agoraio-extensions/terra-legacy-cxx-parser");
  let agora_rtc_ast_dir_path = path.join(
    __dirname,
    "..",
    "cxx",
    "cppast_backend"
  );
  console.log("agora_rtc_ast_dir_path: " + agora_rtc_ast_dir_path);

  let build_shell_path = path.join(agora_rtc_ast_dir_path, "build.sh");
  let build_cache_dir_path = path.join(agora_rtc_ast_dir_path, "build");
  // let outputJsonPath = path.join(build_cache_dir_path, "dump_json.json");

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

  let buildScript = `bash ${build_shell_path} \"${bashArgs}\"`;
  console.log(`Running command: \n${buildScript}`);

  execSync(buildScript, { encoding: "utf8", stdio: "inherit" });
}

// export class LegacyCXXParser extends Parser {
//   private legacyCxxParserConfigs: LegacyCXXParserConfigs;
//   private parseConfig: ParseConfig;

//   public constructor(
//     parseConfig: ParseConfig,
//     cxxParserConfigs: LegacyCXXParserConfigs
//   ) {
//     super(parseConfig);
//     this.parseConfig = parseConfig;

//     this.legacyCxxParserConfigs = LegacyCXXParserConfigs.resolve(
//       parseConfig.configDir,
//       cxxParserConfigs
//     );
//   }

//   override parse(preParseResult?: ParseResult): ParseResult | undefined {
//     let parseFiles = this.legacyCxxParserConfigs.parseFiles.include.filter(
//       (it) => {
//         return !this.legacyCxxParserConfigs.parseFiles.exclude.includes(it);
//       }
//     );
//     runAgoraRtcAstBash(
//       this.legacyCxxParserConfigs.language,
//       this.legacyCxxParserConfigs.includeHeaderDirs,
//       this.legacyCxxParserConfigs.customHeaders,
//       parseFiles,
//       this.legacyCxxParserConfigs.definesMacros,
//       this.legacyCxxParserConfigs.legacyFlags,
//       this.parseConfig.outputDir,
//       this.legacyCxxParserConfigs.legacyRenders
//     );

//     return undefined;
//   }
// }

export function LegacyCXXParser(terraContext: TerraContext, args: any, parseResult?: ParseResult): ParseResult | undefined {
  let legacyCxxParserConfigs = LegacyCXXParserConfigs.resolve(
    terraContext.configDir,
    args
  );

  let parseFiles = legacyCxxParserConfigs.parseFiles.include.filter(
    (it) => {
      return !legacyCxxParserConfigs.parseFiles.exclude.includes(it);
    }
  );
  runAgoraRtcAstBash(
    legacyCxxParserConfigs.language,
    legacyCxxParserConfigs.includeHeaderDirs,
    legacyCxxParserConfigs.customHeaders,
    parseFiles,
    legacyCxxParserConfigs.definesMacros,
    legacyCxxParserConfigs.legacyFlags,
    terraContext.outputDir,
    legacyCxxParserConfigs.legacyRenders
  );

  return undefined;
}
