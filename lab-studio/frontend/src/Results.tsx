import { useState } from 'react';
import { Check, Copy, Download, Terminal, Table2, Braces } from 'lucide-react';
import { Json, Run, download } from './types';
import { Sequence } from './Charts';

export function ScalarCards({run}:{run:Run}) {
  const trace=run.trace&&typeof run.trace==='object'&&!Array.isArray(run.trace)?run.trace:{};
  const entries=Object.entries(trace).filter(([,v])=>v!==null&&typeof v!=='object');
  let metrics:[string,string][]=[];
  if(run.graph?.accepted?.length)metrics.push(['Accepted edges',`${run.graph.accepted.filter(Boolean).length} / ${run.graph.accepted.length}`],['Rejected edges',String(run.graph.accepted.filter(v=>!v).length)]);
  if(run.graph?.counts?.length)metrics.push(['Final island count',String(run.graph.counts.at(-1))]);
  if(run.graph?.maxima?.length)metrics.push(['Final maximum',String(run.graph.maxima.at(-1)??'NONE')]);
  metrics.push(...entries.filter(([k,v])=>String(v).length<90&&!['work','stdout','tree'].includes(k)).slice(0,5).map(([k,v])=>[k,String(v)] as [string,string]));
  if(!metrics.length)for(const line of run.stdout.split('\n')){
    const match=line.match(/^\s*([^:]{2,48})\s*:\s*([^:]{1,65})$/);
    if(match&&!/enter|student|file|data written|complex|recurrence|time|space|pseudocode/i.test(match[1])&&match[2].trim())metrics.push([match[1].trim(),match[2].trim()]);
    if(metrics.length>=6)break;
  }
  const work=entries.find(([k])=>k==='work')?.[1]??run.stdout.match(/Work:\s*(\d+)/)?.[1];
  if(work!==undefined)metrics.push(['Counted work',String(work)]);
  return <div className="metric-list">{metrics.length?metrics.slice(0,7).map(([k,v],i)=><div key={i} className="result-metric"><span>{k.replaceAll('_',' ')}</span><strong title={v}>{v}</strong></div>):<p className="caption">Run completed. Open Results for the full answer.</p>}</div>;
}

function Matrix({value,name}:{value:Json[][];name:string}) {
  const numeric=value.flat().filter(v=>typeof v==='number') as number[];
  const max=numeric.reduce((a,b)=>Math.max(a,Math.abs(b)),1);
  const rows=value.slice(0,70),cols=Math.max(0,...rows.map(r=>r.length));
  return <section className="result-section"><div className="section-title"><h3>{name.replaceAll('_',' ')}</h3><span className="tag">{value.length} × {cols}</span></div><div className="table-scroll matrix-table"><table><thead><tr><th>i / j</th>{Array.from({length:Math.min(cols,70)},(_,i)=><th key={i}>{i}</th>)}</tr></thead><tbody>{rows.map((row,i)=><tr key={i}><th>{i}</th>{row.slice(0,70).map((v,j)=><td key={j} style={typeof v==='number'?{background:`hsl(95 34% ${97-Math.abs(v)/max*23}%)`}:undefined}>{typeof v==='object'?JSON.stringify(v):String(v??'—')}</td>)}</tr>)}</tbody></table></div>{(value.length>70||cols>70)&&<p className="caption">First 70 rows and columns shown. Export JSON for the full table.</p>}</section>;
}

function ArrayResult({value,name}:{value:Json[];name:string}) {
  if(!value.length)return <section className="result-section"><h3>{name.replaceAll('_',' ')}</h3><p className="caption">Empty result.</p></section>;
  if(value.every(Array.isArray))return <Matrix value={value as Json[][]} name={name}/>;
  if(value.every(v=>v!==null&&typeof v==='object'&&!Array.isArray(v))) {
    const keys=Array.from(new Set(value.flatMap(v=>Object.keys(v as object))));
    return <section className="result-section"><h3>{name.replaceAll('_',' ')}</h3><div className="table-scroll"><table><thead><tr><th>#</th>{keys.map(k=><th key={k}>{k.replaceAll('_',' ')}</th>)}</tr></thead><tbody>{value.slice(0,500).map((v,i)=><tr key={i}><th>{i+1}</th>{keys.map(k=><td key={k}>{typeof (v as {[key:string]:Json})[k]==='object'?JSON.stringify((v as {[key:string]:Json})[k]):String((v as {[key:string]:Json})[k]??'—')}</td>)}</tr>)}</tbody></table></div>{value.length>500&&<p className="caption">First 500 rows shown. Export JSON for all rows.</p>}</section>;
  }
  return <section className="result-section"><h3>{name.replaceAll('_',' ')}</h3><div className="value-chips">{value.slice(0,500).map((v,i)=><div key={i}><small>{i}</small><span>{v===null?'NONE':typeof v==='object'?JSON.stringify(v):String(v)}</span></div>)}</div>{value.length>500&&<p className="caption">First 500 values shown. Export JSON for all values.</p>}</section>;
}

export function StructuredResults({run}:{run:Run}) {
  const trace=run.trace;
  let arrays:[string,Json[]][]=[];
  if(trace&&typeof trace==='object'&&!Array.isArray(trace))arrays=Object.entries(trace).filter((entry):entry is [string,Json[]]=>Array.isArray(entry[1]));
  if(run.graph?.accepted?.length)arrays.push(['Edge decisions',run.graph.edges!.map((edge,i)=>({edge:i+1,from:edge[0],to:edge[1],decision:run.graph!.accepted![i]?'ACCEPTED':'REJECTED (odd cycle)'}))]);
  if(run.graph?.colours?.length)arrays.push(['Final bipartition',run.graph.colours.map((colour,i)=>({vertex:i+1,colour}))]);
  if(run.graph?.counts)arrays.push(['Island counts',run.graph.counts.map((count,i)=>({step:i+1,row:run.graph!.cells![i][0],column:run.graph!.cells![i][1],islands:count}))]);
  if(run.graph?.maxima?.length)arrays.push(['Component maxima',run.graph.maxima]);
  if(run.graph?.permutation?.length)arrays.push(['Smallest reachable permutation',run.graph.permutation]);
  if(!arrays.length){
    const pairs=run.stdout.split('\n').flatMap(line=>{const m=line.match(/^\s*([^:]{2,70}):\s*(.+)$/);return m&&!/enter/i.test(m[1])?[{field:m[1].trim(),value:m[2].trim()}]:[];});
    if(pairs.length)arrays=[['Results',pairs]];
  }
  return <div className="structured-results">{arrays.length?arrays.map(([name,value])=><ArrayResult key={name} name={name} value={value}/>):<p className="caption">This program produces a text transcript. Select Console to read it.</p>}</div>;
}

export function Transcript({text}:{text:string}) {
  const [copied,setCopied]=useState(false);
  return <div className="transcript"><div className="transcript-header"><span><Terminal size={14}/> PROGRAM OUTPUT</span><button onClick={async()=>{try{await navigator.clipboard.writeText(text);setCopied(true);setTimeout(()=>setCopied(false),1500);}catch{download('output.txt',text);}}}>{copied?<Check size={14}/>:<Copy size={14}/>} {copied?'Copied':'Copy'}</button></div><pre>{text}</pre></div>;
}

export function ResultPanel({run}:{run:Run}) {
  const [mode,setMode]=useState('table');
  return <div className="result-panel"><div className="subtabs"><div><button className={mode==='table'?'selected':''} onClick={()=>setMode('table')}><Table2 size={15}/> Tables</button><button className={mode==='console'?'selected':''} onClick={()=>setMode('console')}><Terminal size={15}/> Console</button><button className={mode==='json'?'selected':''} onClick={()=>setMode('json')}><Braces size={15}/> JSON</button></div><button onClick={()=>download(`${run.problemId}-result.json`,JSON.stringify(run,null,2),'application/json')}><Download size={15}/> Export</button></div>{mode==='table'?<StructuredResults run={run}/>:<Transcript text={mode==='json'?JSON.stringify(run,null,2):run.stdout+(run.stderr?'\n'+run.stderr:'')}/>}</div>;
}

export function TraceVisual({run}:{run:Run}) {
  const obj=run.trace&&typeof run.trace==='object'&&!Array.isArray(run.trace)?run.trace:{};
  const matrices=Object.entries(obj).filter((entry):entry is [string,Json[][]]=>Array.isArray(entry[1])&&entry[1].length>0&&entry[1].every(Array.isArray));
  if(matrices.length)return <div className="trace-visual">{matrices.map(([name,value])=><Matrix key={name} name={name} value={value}/>)}</div>;
  const sequences=Object.entries(obj).filter((entry):entry is [string,(number|string|null)[]]=>Array.isArray(entry[1])&&entry[1].length>0&&entry[1].every(v=>v===null||typeof v==='number'||typeof v==='string'));
  if(sequences.length)return <div className="trace-visual">{sequences.map(([name,value])=><Sequence key={name} title={name.replaceAll('_',' ')} values={value.slice(0,2000)}/>)}</div>;
  // Parse labelled numeric arrays from the actual C transcript for legacy programs.
  const lines=run.stdout.split('\n'),arrays:[string,number[]][]=[];
  for(let i=0;i<lines.length;i++){
    const pieces=lines[i].split(':');
    const numeric=(pieces.length>1?pieces.slice(1).join(':'):lines[i]).trim();
    if(/^[-+\d.eE\s]+$/.test(numeric)){
      const numbers=numeric.split(/\s+/).map(Number);
      if(numbers.length>1&&numbers.every(Number.isFinite))arrays.push([pieces.length>1?pieces[0].trim():lines[i-1]?.replace(/:$/,'').trim()||'Result values',numbers]);
    }
  }
  if(arrays.length)return <div className="trace-visual">{arrays.slice(0,4).map(([name,value],i)=><Sequence key={i} title={name||'Result values'} values={value}/>)}</div>;
  return <div className="trace-visual"><ScalarCards run={run}/><StructuredResults run={run}/><Transcript text={run.stdout}/></div>;
}
