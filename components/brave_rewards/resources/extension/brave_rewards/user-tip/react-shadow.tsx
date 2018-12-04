import * as React from 'react'
import { createPortal } from 'react-dom'


type Props = {
  mode: 'open' | 'closed'
}

export default class ShadowReactDOM extends React.Component<Props> {

  node?: any
  appContainer?: Element
  stylesContainer?: Element
  defaultProps = {
    mode: 'closed'
  }

  componentDidMount () {
    // @ts-ignore
    const shadowRoot = this.node.attachShadow({ mode: this.props.mode })
    this.appContainer = document.createElement('div')
    this.stylesContainer = document.createElement('div')
    shadowRoot.appendChild(this.appContainer)
    shadowRoot.appendChild(this.stylesContainer)
    this.forceUpdate()
  }

  getRef = (node: any) => {
    this.node = node
  }

  render () {
    const { children, ...props } = this.props
    return (
      <>
        {
          this.appContainer &&
          <StyleSheetManager target={this.stylesContainer}>
            <div {...props} ref={this.getRef}>
                {
                  this.appContainer &&
                  createPortal(children, this.appContainer)
                }
            </div>
          </StyleSheetManager>
        }
      </>
    )
  }
}
