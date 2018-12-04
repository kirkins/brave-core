import * as React from 'react'
import { Tip } from 'brave-ui/features/rewards'

const donationAmounts = [
  {
    tokens: '1.0',
    converted: '0.30',
    selected: false
  },
  {
    tokens: '5.0',
    converted: '1.50',
    selected: false
  },
  {
    tokens: '10.0',
    converted: '3.00',
    selected: false
  }
]

export default class TipDialogContent extends React.Component {

  onDonate = () => {
    console.log('does nothing')
  }

  onClose = () => {
    console.log('does nothing')
  }

  onAllow = () => {
    console.log('does nothing')
  }

  onAmountSelection = () => {
    console.log('does nothing')
  }

  render () {
    return (
      <Tip
        donationAmounts={donationAmounts}
        title='Bart Baker'
        allow={true}
        provider='YouTube'
        balance='5'
        onDonate={this.onDonate}
        onClose={this.onClose}
        onAllow={this.onAllow}
        onAmountSelection={this.onAmountSelection}
        currentAmount={5}
      />
    )
  }
}